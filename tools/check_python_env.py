#!/usr/bin/env python3
"""Check the Python environment used by TinyPredict-STM32 tools."""

import subprocess
import sys


def print_pip_info() -> None:
    print("pip check:")
    try:
        result = subprocess.run(
            [sys.executable, "-m", "pip", "--version"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
            timeout=10,
        )
    except Exception as exc:
        print(f"  FAILED to run pip with current Python: {exc}")
        print("  Try installing/enabling pip for this Python environment.")
        return

    if result.returncode == 0:
        print(f"  {result.stdout.strip()}")
    else:
        print("  FAILED")
        if result.stderr.strip():
            print(f"  stderr: {result.stderr.strip()}")
        if result.stdout.strip():
            print(f"  stdout: {result.stdout.strip()}")


def main() -> int:
    print("TinyPredict-STM32 Python environment check")
    print(f"sys.executable: {sys.executable}")
    print(f"sys.version: {sys.version}")
    print_pip_info()

    try:
        import serial
    except ImportError:
        print("pyserial import: FAILED")
        print("The current Python cannot import the 'serial' module.")
        print("Install pyserial using the same Python executable shown above:")
        print("  python -m pip install -r tools/requirements.txt")
        print("or, on Windows with the Python launcher:")
        print("  py -m pip install -r tools/requirements.txt")
        print("If you want to force a specific Python in Git Bash, use a full path, for example:")
        print("  /c/Users/16super/AppData/Local/Programs/Python/Python313/python.exe -m pip install -r tools/requirements.txt")
        return 1

    version = getattr(serial, "__version__", "unknown")
    module_path = getattr(serial, "__file__", "unknown")
    print("pyserial import: OK")
    print(f"serial.__version__: {version}")
    print(f"serial module path: {module_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
