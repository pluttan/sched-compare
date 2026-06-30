"""Shared matplotlib styling for the presentation."""
from __future__ import annotations

import matplotlib as mpl
import matplotlib.pyplot as plt

CFS_COLOR = "#E85D75"
EEVDF_COLOR = "#4CC9F0"
ACCENT = "#FFD166"
MUTED = "#8D99AE"
BG = "#0F1620"
FG = "#E8ECF1"


def apply_dark_theme() -> None:
    mpl.rcParams.update(
        {
            "figure.facecolor": BG,
            "axes.facecolor": BG,
            "savefig.facecolor": BG,
            "axes.edgecolor": FG,
            "axes.labelcolor": FG,
            "xtick.color": FG,
            "ytick.color": FG,
            "text.color": FG,
            "axes.titlesize": 14,
            "axes.titleweight": "bold",
            "axes.labelsize": 12,
            "legend.edgecolor": FG,
            "legend.facecolor": BG,
            "legend.labelcolor": FG,
            "grid.color": "#2A2F3A",
            "grid.linestyle": "--",
            "grid.alpha": 0.7,
            "figure.dpi": 110,
            "savefig.dpi": 160,
            "font.family": "DejaVu Sans",
        }
    )


def color_for(algo: str) -> str:
    return CFS_COLOR if algo.lower() == "cfs" else EEVDF_COLOR
