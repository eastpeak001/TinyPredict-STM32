#!/usr/bin/env python3
"""Serial CSV logger for TinyPredict-STM32.

Preferred STM32 firmware output:
    time_ms,ax,ay,az,rms,status
    1234,0.012,-0.034,1.002,0.056,NORMAL

Legacy key-value lines are also parsed when possible:
    t=1234 ax=0.012 ay=-0.034 az=1.002 rms=0.056 status=NORMAL
"""

import argparse
import csv
import re
import sys
import time
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional

PROJECT_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_DATA_DIR = PROJECT_ROOT / "docs" / "data"
CSV_FIELDS = ["time_ms", "ax", "ay", "az", "rms", "status"]
NUMERIC_FIELDS = ["time_ms", "ax", "ay", "az", "rms"]
FIRMWARE_HEADER = ",".join(CSV_FIELDS)
KEY_VALUE_PATTERN = re.compile(r"\b(time_ms|t|ax|ay|az|rms|status)\s*=\s*([^,\s]+)")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Log TinyPredict-STM32 USART CSV output to a CSV file."
    )
    parser.add_argument(
        "--port",
        default=None,
        help="Serial port name, for example COM5. If omitted, the tool lists available ports.",
    )
    parser.add_argument(
        "--baud",
        type=int,
        default=115200,
        help="Serial baud rate. Default: 115200.",
    )
    parser.add_argument(
        "--out",
        "--output",
        dest="out",
        default=None,
        help="CSV output path. Default: docs/data/log_YYYYMMDD_HHMMSS.csv.",
    )
    parser.add_argument(
        "--label",
        default=None,
        help="Legacy option kept for old scripts. Labels are not written to the current CSV format.",
    )
    parser.add_argument(
        "--list-ports",
        action="store_true",
        help="List available serial ports and exit.",
    )
    return parser.parse_args()


def make_default_output_path() -> Path:
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    return DEFAULT_DATA_DIR / f"log_{timestamp}.csv"


def import_serial_module():
    try:
        import serial
    except ImportError as exc:
        message = (
            "Failed to import the 'serial' module from pyserial.\n"
            f"Current Python executable: {sys.executable}\n"
            f"Current Python version: {sys.version}\n"
            "Install pyserial into this same Python environment with one of these commands:\n"
            "  python -m pip install -r tools/requirements.txt\n"
            "  py -m pip install -r tools/requirements.txt\n"
            "You can also run: python tools\\check_python_env.py"
        )
        raise RuntimeError(message) from exc

    return serial


def list_serial_ports() -> List[object]:
    try:
        from serial.tools import list_ports
    except ImportError as exc:
        raise RuntimeError(
            "Failed to import serial.tools.list_ports. Please install pyserial."
        ) from exc

    return list(list_ports.comports())


def print_no_serial_ports_help() -> None:
    print("No serial ports detected.")
    print("")
    print("Please check:")
    print("1. Is the USB-to-TTL adapter plugged in?")
    print("2. Is the USB cable a data cable, not charge-only?")
    print("3. Open Windows Device Manager -> Ports (COM & LPT)")
    print("4. Check whether USB-SERIAL CH340 / CP210x / USB Serial Port appears")
    print("5. If no COM port appears, install the correct USB-to-TTL driver")
    print("6. Try another USB port or cable")
    print("")
    print("Common drivers:")
    print("- CH340 / CH341: WCH CH341SER driver")
    print("- CP2102: Silicon Labs CP210x VCP driver")


def print_available_ports(ports: List[object]) -> None:
    if not ports:
        print_no_serial_ports_help()
        return

    print("Available serial ports:")
    for index, port in enumerate(ports, start=1):
        description = port.description or "Unknown device"
        print(f"{index}. {port.device} - {description}")


def resolve_serial_port(requested_port: Optional[str]) -> Optional[str]:
    ports = list_serial_ports()

    if requested_port:
        for port in ports:
            if port.device.upper() == requested_port.upper():
                return port.device

        print(f"Serial port {requested_port} not found.", file=sys.stderr)
        print("Please check available ports below:", file=sys.stderr)
        if ports:
            for port in ports:
                description = port.description or "Unknown device"
                print(f"{port.device} - {description}", file=sys.stderr)
        else:
            print_no_serial_ports_help()
        return None

    print_available_ports(ports)

    if len(ports) == 1:
        selected_port = ports[0].device
        print(f"Auto selected serial port: {selected_port}")
        return selected_port

    if len(ports) > 1:
        print("Please specify the target port, for example:")
        print(f"  python tools/serial_logger.py --port {ports[0].device} --baud 115200")
    else:
        print("Please connect the USB-to-TTL adapter and try again.")

    return None


def is_firmware_header(line: str) -> bool:
    normalized = line.strip().replace(" ", "").lower()
    return normalized == FIRMWARE_HEADER


def normalize_row(row: Dict[str, object]) -> Optional[Dict[str, object]]:
    normalized: Dict[str, object] = {}

    for field in CSV_FIELDS:
        value = row.get(field)
        if value is None:
            return None
        if isinstance(value, str):
            value = value.strip()
        normalized[field] = value

    for field in NUMERIC_FIELDS:
        try:
            number = float(normalized[field])
        except (TypeError, ValueError):
            return None

        if field == "time_ms":
            normalized[field] = str(int(number))
        else:
            normalized[field] = f"{number:.3f}"

    normalized["status"] = str(normalized["status"]).strip()
    if not normalized["status"]:
        return None

    return normalized


def parse_csv_data_line(line: str) -> Optional[Dict[str, object]]:
    try:
        values = next(csv.reader([line]))
    except csv.Error:
        return None

    if len(values) != len(CSV_FIELDS):
        return None

    row = dict(zip(CSV_FIELDS, values))
    return normalize_row(row)


def parse_legacy_key_value_line(line: str, fallback_time_ms: int) -> Optional[Dict[str, object]]:
    values = {key: value for key, value in KEY_VALUE_PATTERN.findall(line)}
    required_fields = ["ax", "ay", "az", "rms", "status"]
    if not all(field in values for field in required_fields):
        return None

    time_value = values.get("time_ms", values.get("t", str(fallback_time_ms)))
    row: Dict[str, object] = {
        "time_ms": time_value,
        "ax": values["ax"],
        "ay": values["ay"],
        "az": values["az"],
        "rms": values["rms"],
        "status": values["status"],
    }
    return normalize_row(row)


def parse_serial_line(line: str, fallback_time_ms: int) -> Optional[Dict[str, object]]:
    if not line:
        return None

    if is_firmware_header(line):
        return None

    csv_row = parse_csv_data_line(line)
    if csv_row is not None:
        return csv_row

    return parse_legacy_key_value_line(line, fallback_time_ms)


def open_serial(port: str, baud: int):
    serial = import_serial_module()
    return serial.Serial(port=port, baudrate=baud, timeout=1.0)


def print_startup_info(port: str, baud: int, output_path: Path) -> None:
    print(f"Serial port: {port}")
    print(f"Baud rate: {baud}")
    print(f"Output CSV: {output_path}")
    print(f"Expected CSV fields: {FIRMWARE_HEADER}")


def main() -> int:
    args = parse_args()

    try:
        if args.list_ports:
            print_available_ports(list_serial_ports())
            return 0

        selected_port = resolve_serial_port(args.port)
    except Exception as exc:
        print(f"Serial port check failed: {exc}", file=sys.stderr)
        return 1

    if selected_port is None:
        return 1

    output_path = Path(args.out) if args.out else make_default_output_path()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    print_startup_info(selected_port, args.baud, output_path)

    serial_port = None
    try:
        serial_port = open_serial(selected_port, args.baud)
    except Exception as exc:
        print(f"Failed to open serial port {selected_port}: {exc}", file=sys.stderr)
        print("Please check that the COM port exists and is not used by another program.", file=sys.stderr)
        return 1

    start_monotonic = time.monotonic()

    try:
        with output_path.open("a", newline="", encoding="utf-8") as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=CSV_FIELDS)
            if output_path.stat().st_size == 0:
                writer.writeheader()
                csv_file.flush()

            print("Logging started. Press Ctrl+C to stop.")
            while True:
                try:
                    raw_line = serial_port.readline()
                except Exception as exc:
                    print(f"Serial read failed: {exc}", file=sys.stderr)
                    return 1

                if not raw_line:
                    continue

                line = raw_line.decode("utf-8", errors="replace").strip()
                fallback_time_ms = int((time.monotonic() - start_monotonic) * 1000)
                row = parse_serial_line(line, fallback_time_ms)
                if row is None:
                    print(f"[info] {line}")
                    continue

                writer.writerow(row)
                csv_file.flush()
                print(f"time_ms={row['time_ms']}, rms={row['rms']}, status={row['status']}")

    except KeyboardInterrupt:
        print("\nStopping logger...")
    except OSError as exc:
        print(f"CSV file error: {exc}", file=sys.stderr)
        return 1
    finally:
        if serial_port is not None:
            try:
                serial_port.close()
                print("Serial port closed.")
            except Exception as exc:
                print(f"Failed to close serial port: {exc}", file=sys.stderr)
        print(f"CSV saved to: {output_path}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
