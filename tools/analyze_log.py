#!/usr/bin/env python3
"""Analyze TinyPredict-STM32 CSV logs and generate plots/reports."""

import argparse
import sys
from pathlib import Path
from typing import Dict, List

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd

PROJECT_ROOT = Path(__file__).resolve().parents[1]
DATA_DIR = PROJECT_ROOT / "docs" / "data"
IMAGE_DIR = PROJECT_ROOT / "docs" / "images" / "analysis"
REQUIRED_COLUMNS = ["time_ms", "ax", "ay", "az", "rms", "status"]
PREFERRED_STATUS_ORDER = ["CALIBRATING", "NORMAL", "WARNING", "ALARM"]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Analyze TinyPredict-STM32 CSV logs and generate RMS/acceleration/status reports."
    )
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--file", help="CSV file to analyze, for example docs/data/test.csv.")
    source.add_argument(
        "--latest",
        action="store_true",
        help="Analyze the newest docs/data/log_*.csv file.",
    )
    return parser.parse_args()


def find_latest_log() -> Path:
    logs = sorted(DATA_DIR.glob("log_*.csv"), key=lambda path: path.stat().st_mtime, reverse=True)
    if not logs:
        raise FileNotFoundError(f"No log_*.csv files found in {DATA_DIR}")
    return logs[0]


def resolve_input_file(args: argparse.Namespace) -> Path:
    if args.latest:
        return find_latest_log()

    csv_path = Path(args.file)
    if not csv_path.is_absolute():
        csv_path = PROJECT_ROOT / csv_path
    return csv_path


def load_csv(csv_path: Path) -> pd.DataFrame:
    if not csv_path.exists():
        raise FileNotFoundError(f"CSV file not found: {csv_path}")

    try:
        df = pd.read_csv(csv_path)
    except pd.errors.EmptyDataError as exc:
        raise ValueError(f"CSV file is empty: {csv_path}") from exc
    except Exception as exc:
        raise ValueError(f"Failed to read CSV file {csv_path}: {exc}") from exc

    missing = [column for column in REQUIRED_COLUMNS if column not in df.columns]
    if missing:
        raise ValueError(
            "CSV missing required columns: "
            + ", ".join(missing)
            + f". Required columns: {', '.join(REQUIRED_COLUMNS)}"
        )

    if df.empty:
        raise ValueError("CSV contains only the header and no data rows.")

    df = df[REQUIRED_COLUMNS].copy()
    for column in ["time_ms", "ax", "ay", "az", "rms"]:
        df[column] = pd.to_numeric(df[column], errors="coerce")

    invalid_rows = df[df[["time_ms", "ax", "ay", "az", "rms"]].isna().any(axis=1)]
    if not invalid_rows.empty:
        raise ValueError(
            f"CSV contains {len(invalid_rows)} rows with invalid numeric values. "
            "Please check time_ms, ax, ay, az and rms columns."
        )

    df["status"] = df["status"].astype(str).str.strip()
    df["time_s"] = (df["time_ms"] - df["time_ms"].iloc[0]) / 1000.0
    return df


def output_paths(csv_path: Path) -> Dict[str, Path]:
    IMAGE_DIR.mkdir(parents=True, exist_ok=True)
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    stem = csv_path.stem
    return {
        "rms": IMAGE_DIR / f"{stem}_rms.png",
        "accel": IMAGE_DIR / f"{stem}_accel.png",
        "status": IMAGE_DIR / f"{stem}_status_count.png",
        "report": DATA_DIR / f"{stem}_report.md",
    }


def save_rms_plot(df: pd.DataFrame, path: Path) -> None:
    plt.figure(figsize=(10, 4.8))
    plt.plot(df["time_s"], df["rms"], label="RMS", color="#1f77b4", linewidth=1.5)
    plt.axhline(0.05, color="#ffb000", linestyle="--", linewidth=1.0, label="WARNING threshold 0.05")
    plt.axhline(0.15, color="#d62728", linestyle="--", linewidth=1.0, label="ALARM threshold 0.15")
    plt.title("TinyPredict RMS Curve")
    plt.xlabel("Time (s)")
    plt.ylabel("RMS (g)")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(path, dpi=150)
    plt.close()


def save_accel_plot(df: pd.DataFrame, path: Path) -> None:
    plt.figure(figsize=(10, 4.8))
    plt.plot(df["time_s"], df["ax"], label="ax", linewidth=1.2)
    plt.plot(df["time_s"], df["ay"], label="ay", linewidth=1.2)
    plt.plot(df["time_s"], df["az"], label="az", linewidth=1.2)
    plt.title("TinyPredict Acceleration Curve")
    plt.xlabel("Time (s)")
    plt.ylabel("Acceleration (g)")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(path, dpi=150)
    plt.close()


def ordered_status_counts(df: pd.DataFrame) -> pd.Series:
    counts = df["status"].value_counts()
    ordered_labels: List[str] = [status for status in PREFERRED_STATUS_ORDER if status in counts.index]
    ordered_labels.extend(sorted(status for status in counts.index if status not in ordered_labels))
    return counts.reindex(ordered_labels).fillna(0).astype(int)


def save_status_plot(counts: pd.Series, path: Path) -> None:
    colors = {
        "CALIBRATING": "#7f7f7f",
        "NORMAL": "#2ca02c",
        "WARNING": "#ffb000",
        "ALARM": "#d62728",
    }
    bar_colors = [colors.get(label, "#1f77b4") for label in counts.index]

    plt.figure(figsize=(7, 4.8))
    plt.bar(counts.index, counts.values, color=bar_colors)
    plt.title("TinyPredict Status Counts")
    plt.xlabel("Status")
    plt.ylabel("Sample Count")
    plt.grid(axis="y", alpha=0.3)
    plt.tight_layout()
    plt.savefig(path, dpi=150)
    plt.close()


def relative_link(from_file: Path, target: Path) -> str:
    if from_file.parent.name == "data":
        return f"../images/analysis/{target.name}"
    return target.as_posix()


def automatic_conclusion(counts: pd.Series) -> str:
    alarm_count = int(counts.get("ALARM", 0))
    warning_count = int(counts.get("WARNING", 0))
    normal_count = int(counts.get("NORMAL", 0))
    total_count = int(counts.sum())

    if alarm_count > 0:
        return "本次数据中出现 ALARM 状态，说明测试过程中存在明显报警状态。"
    if warning_count > 0:
        return "本次数据中出现 WARNING 状态且未出现 ALARM，说明存在轻微异常或振动增大。"
    if normal_count == total_count:
        return "本次数据全部为 NORMAL，整体运行状态较平稳。"
    return "本次数据未出现 WARNING/ALARM，但包含非 NORMAL 状态，建议结合原始数据进一步检查。"


def write_report(csv_path: Path, df: pd.DataFrame, counts: pd.Series, paths: Dict[str, Path]) -> None:
    duration_s = float(df["time_s"].iloc[-1]) if len(df) > 1 else 0.0
    too_short_note = "\n> Note: 数据点少于 2 个，采样时长按 0 秒处理。\n" if len(df) < 2 else ""
    report_path = paths["report"]

    status_lines = "\n".join(f"| {status} | {count} |" for status, count in counts.items())
    accel_lines = "\n".join(
        f"| {axis} | {df[axis].min():.3f} | {df[axis].max():.3f} |" for axis in ["ax", "ay", "az"]
    )

    content = f"""# TinyPredict CSV Analysis Report

## Data File

`{csv_path}`

## Summary

- Sample count: {len(df)}
- Duration: {duration_s:.3f} s
- RMS max: {df['rms'].max():.3f}
- RMS mean: {df['rms'].mean():.3f}
{too_short_note}
## Acceleration Range

| Axis | Min (g) | Max (g) |
| --- | ---: | ---: |
{accel_lines}

## Status Counts

| Status | Count |
| --- | ---: |
{status_lines}

## Plots

![RMS Curve]({relative_link(report_path, paths['rms'])})

![Acceleration Curve]({relative_link(report_path, paths['accel'])})

![Status Counts]({relative_link(report_path, paths['status'])})

## Automatic Conclusion

{automatic_conclusion(counts)}
"""
    report_path.write_text(content, encoding="utf-8")


def analyze(csv_path: Path) -> Dict[str, Path]:
    df = load_csv(csv_path)
    paths = output_paths(csv_path)
    counts = ordered_status_counts(df)

    save_rms_plot(df, paths["rms"])
    save_accel_plot(df, paths["accel"])
    save_status_plot(counts, paths["status"])
    write_report(csv_path, df, counts, paths)
    return paths


def main() -> int:
    args = parse_args()

    try:
        csv_path = resolve_input_file(args)
        paths = analyze(csv_path)
    except Exception as exc:
        print(f"Analyze failed: {exc}", file=sys.stderr)
        return 1

    print(f"Analyzed CSV: {csv_path}")
    print(f"RMS plot: {paths['rms']}")
    print(f"Acceleration plot: {paths['accel']}")
    print(f"Status count plot: {paths['status']}")
    print(f"Markdown report: {paths['report']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

