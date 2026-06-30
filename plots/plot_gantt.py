"""Gantt chart comparing CFS vs EEVDF task-on-CPU timelines for the mixed trace."""
from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

from style import apply_dark_theme, CFS_COLOR, EEVDF_COLOR, ACCENT, MUTED


def runs_from_events(df: pd.DataFrame) -> list[dict]:
    """Turn start/preempt/finish events into concrete on-CPU spans."""
    runs = []
    running_start = None
    running_task = None
    for _, row in df.iterrows():
        ev = row["event"]
        t = row["time_ms"]
        name = row["task_name"]
        if ev == "start":
            running_start = t
            running_task = name
        elif ev in ("preempt", "finish") and running_start is not None:
            runs.append({"start": running_start, "end": t, "name": running_task})
            running_start = None
            running_task = None
    return runs


def draw(ax, df: pd.DataFrame, title: str, task_order: list[str], *, highlight: str | None = None):
    runs = runs_from_events(df)
    idx = {name: i for i, name in enumerate(task_order)}
    for r in runs:
        y = idx.get(r["name"])
        if y is None:
            continue
        color = ACCENT if r["name"] == highlight else MUTED
        ax.barh(y, r["end"] - r["start"], left=r["start"], height=0.7,
                color=color, edgecolor=color)

    arrivals = df[df["event"] == "arrival"]
    for _, row in arrivals.iterrows():
        y = idx.get(row["task_name"])
        if y is None:
            continue
        ax.plot(row["time_ms"], y, marker="v", color="w", markersize=8, clip_on=False)

    ax.set_yticks(range(len(task_order)))
    ax.set_yticklabels(task_order)
    ax.set_xlabel("time, ms")
    ax.set_title(title)
    ax.grid(True, axis="x")


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--cfs", default="results/cfs_mixed.csv")
    ap.add_argument("--eevdf", default="results/eevdf_mixed.csv")
    ap.add_argument("--highlight", default="interactive")
    ap.add_argument("--xmax", type=float, default=70.0)
    ap.add_argument("--out", default="plots/out/gantt_mixed.png")
    args = ap.parse_args()

    apply_dark_theme()

    cfs_df = pd.read_csv(args.cfs)
    eevdf_df = pd.read_csv(args.eevdf)

    task_order = list(dict.fromkeys(cfs_df["task_name"].tolist()))

    fig, axes = plt.subplots(2, 1, figsize=(11, 5), sharex=True)
    draw(axes[0], cfs_df, "CFS", task_order, highlight=args.highlight)
    draw(axes[1], eevdf_df, "EEVDF", task_order, highlight=args.highlight)
    axes[0].set_xlim(0, args.xmax)
    axes[0].spines["bottom"].set_color(CFS_COLOR)
    axes[1].spines["bottom"].set_color(EEVDF_COLOR)
    fig.suptitle(f'On-CPU timeline · "{args.highlight}" wakes at t=50ms', fontsize=15)
    fig.tight_layout()

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(out)
    print(f"wrote {out}")


if __name__ == "__main__":
    main()
