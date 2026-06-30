"""CDF of wake-to-first-run latency for latency-sensitive tasks across traces."""
from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from style import apply_dark_theme, CFS_COLOR, EEVDF_COLOR


def wait_times(df: pd.DataFrame, task_prefix: str) -> np.ndarray:
    mask = df["task_name"].str.startswith(task_prefix)
    sub = df[mask]
    rows = []
    for name, grp in sub.groupby("task_name"):
        arr = grp[grp["event"] == "arrival"]["time_ms"]
        start = grp[grp["event"] == "start"]["time_ms"]
        if arr.empty or start.empty:
            continue
        rows.append(start.iloc[0] - arr.iloc[0])
    return np.array(rows, dtype=float)


def cdf(values: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    x = np.sort(values)
    y = np.arange(1, len(x) + 1) / len(x)
    return x, y


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--results-dir", default="results")
    ap.add_argument("--traces", nargs="+", default=["bursty", "latency_nice"])
    ap.add_argument("--prefix", default="wake")
    ap.add_argument("--out", default="plots/out/wait_cdf.png")
    args = ap.parse_args()

    apply_dark_theme()

    cfs_all, eevdf_all = [], []
    for trace in args.traces:
        cfs_df = pd.read_csv(Path(args.results_dir) / f"cfs_{trace}.csv")
        eevdf_df = pd.read_csv(Path(args.results_dir) / f"eevdf_{trace}.csv")
        cfs_all.extend(wait_times(cfs_df, args.prefix).tolist())
        eevdf_all.extend(wait_times(eevdf_df, args.prefix).tolist())
    cfs_all = np.array(cfs_all)
    eevdf_all = np.array(eevdf_all)

    fig, ax = plt.subplots(figsize=(9, 5))
    for values, label, color in [
        (cfs_all, "CFS", CFS_COLOR),
        (eevdf_all, "EEVDF", EEVDF_COLOR),
    ]:
        x, y = cdf(values)
        ax.step(x, y, where="post", color=color, linewidth=2.5, label=label)

    ax.set_xlabel("wake → first run, ms")
    ax.set_ylabel("CDF")
    ax.set_title(f'Wakeup latency CDF · latency-sensitive tasks (prefix "{args.prefix}")')
    ax.grid(True)
    ax.legend(loc="lower right", fontsize=11)

    def pct(v, p):
        return float(np.percentile(v, p)) if len(v) else float("nan")

    summary = (
        f"N={len(cfs_all)}  "
        f"CFS p50={pct(cfs_all,50):.2f}ms p95={pct(cfs_all,95):.2f}ms  "
        f"EEVDF p50={pct(eevdf_all,50):.2f}ms p95={pct(eevdf_all,95):.2f}ms"
    )
    ax.text(0.02, 0.95, summary, transform=ax.transAxes, fontsize=9,
            verticalalignment="top", color="#CCCCCC")

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    fig.tight_layout()
    fig.savefig(out)
    print(f"wrote {out}")
    print(summary)


if __name__ == "__main__":
    main()
