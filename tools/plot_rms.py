#!/usr/bin/env python3
"""Plot segmented TinyPredict-STM32 RMS CSV logs."""

import argparse
import csv
import sys
from pathlib import Path
from typing import List, Tuple

SEGMENT_GAP = 8
WARNING_THRESHOLD = 0.05
ALARM_THRESHOLD = 0.15
DEFAULT_OUTPUT = "docs/images/rms_test_curve.png"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Plot RMS curves from one or more TinyPredict CSV files.")
    parser.add_argument("--csv", nargs="+", required=True, help="CSV files to plot in order.")
    parser.add_argument("--output", default=DEFAULT_OUTPUT, help=f"Output image path. Default: {DEFAULT_OUTPUT}.")
    parser.add_argument("--show", action="store_true", help="Show the plot window after saving.")
    return parser.parse_args()


def read_segment(csv_path: Path) -> Tuple[str, List[float]]:
    rms_values: List[float] = []
    label = csv_path.stem

    with csv_path.open("r", newline="", encoding="utf-8") as csv_file:
        reader = csv.DictReader(csv_file)
        for row in reader:
            row_label = (row.get("label") or "").strip()
            if row_label:
                label = row_label

            try:
                rms_values.append(float(row.get("rms", "")))
            except ValueError:
                continue

    return label, rms_values


def build_plot_data(csv_paths: List[Path]) -> Tuple[List[int], List[float], List[Tuple[int, str]]]:
    x_values: List[int] = []
    y_values: List[float] = []
    segment_starts: List[Tuple[int, str]] = []
    x_index = 0

    for csv_path in csv_paths:
        label, rms_values = read_segment(csv_path)
        if not rms_values:
            print(f"Skipped empty or invalid CSV: {csv_path}", file=sys.stderr)
            continue

        segment_starts.append((x_index, label))
        for rms_value in rms_values:
            x_values.append(x_index)
            y_values.append(rms_value)
            x_index += 1

        x_index += SEGMENT_GAP

    return x_values, y_values, segment_starts


def main() -> int:
    args = parse_args()
    csv_paths = [Path(path) for path in args.csv]

    missing_files = [str(path) for path in csv_paths if not path.exists()]
    if missing_files:
        print("Missing CSV files:", file=sys.stderr)
        for path in missing_files:
            print(f"  {path}", file=sys.stderr)
        return 1

    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("matplotlib is required for plotting.", file=sys.stderr)
        print("Install it with: py -m pip install matplotlib", file=sys.stderr)
        return 1

    x_values, y_values, segment_starts = build_plot_data(csv_paths)
    if not x_values:
        print("No valid RMS data found.", file=sys.stderr)
        return 1

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    fig, ax = plt.subplots(figsize=(12, 5))
    ax.plot(x_values, y_values, marker="o", linewidth=1.2, markersize=3, label="RMS")
    ax.axhline(WARNING_THRESHOLD, color="#d99000", linestyle="--", linewidth=1.1, label="WARNING threshold 0.05")
    ax.axhline(ALARM_THRESHOLD, color="#cc3333", linestyle="--", linewidth=1.1, label="ALARM threshold 0.15")
    ax.set_title("TinyPredict-STM32 Segmented RMS Test")
    ax.set_xlabel("Sample index (segmented logs with gaps)")
    ax.set_ylabel("RMS")
    ax.grid(True, linestyle="--", alpha=0.35)
    ax.legend(loc="upper left")

    y_top = max(max(y_values), ALARM_THRESHOLD)
    if y_top <= 0.0:
        y_top = 1.0

    for x_start, label in segment_starts:
        ax.axvline(x_start, color="gray", linestyle="--", linewidth=0.8, alpha=0.6)
        ax.text(x_start, y_top * 1.06, label, rotation=30, ha="left", va="bottom")

    ax.set_ylim(bottom=0.0, top=y_top * 1.25)
    fig.tight_layout()
    fig.savefig(output_path, dpi=150)
    print(f"Saved RMS plot: {output_path}")

    if args.show:
        plt.show()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
