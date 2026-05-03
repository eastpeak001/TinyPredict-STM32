#!/usr/bin/env python3
"""Plot the latest V0.2 segmented TinyPredict-STM32 CSV files."""

import sys
from pathlib import Path
from typing import Dict, List

LABELS = ["idle", "normal", "slight_unbalance", "severe_unbalance"]
OUTPUT_PATH = Path("docs/images/rms_test_curve.png")


def find_project_root() -> Path:
    return Path(__file__).resolve().parents[1]


def find_latest_csv(project_root: Path, label: str) -> Path:
    preferred_pattern = f"tinypredict_{label}_*.csv"
    candidates = list(project_root.glob(preferred_pattern))

    if not candidates:
        candidates = [path for path in project_root.glob("*.csv") if label in path.name]

    if not candidates:
        raise FileNotFoundError(f"Missing CSV for label: {label}")

    return max(candidates, key=lambda path: path.stat().st_mtime)


def main() -> int:
    project_root = find_project_root()
    sys.path.insert(0, str(project_root / "tools"))

    try:
        import plot_rms
    except ImportError as exc:
        print(f"Failed to import plot_rms.py: {exc}", file=sys.stderr)
        return 1

    selected_paths: List[Path] = []
    try:
        for label in LABELS:
            csv_path = find_latest_csv(project_root, label)
            selected_paths.append(csv_path)
            print(f"Using {label}: {csv_path.name}")
    except FileNotFoundError as exc:
        print(str(exc), file=sys.stderr)
        print("Required labels: idle, normal, slight_unbalance, severe_unbalance", file=sys.stderr)
        return 1

    old_argv = sys.argv[:]
    try:
        sys.argv = [
            "plot_rms.py",
            "--csv",
            *[str(path) for path in selected_paths],
            "--output",
            str(project_root / OUTPUT_PATH),
        ]
        return plot_rms.main()
    finally:
        sys.argv = old_argv


if __name__ == "__main__":
    raise SystemExit(main())
