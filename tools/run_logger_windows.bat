@echo off
setlocal
cd /d "%~dp0.."

echo [1/5] Checking current python environment...
python tools\check_python_env.py

echo.
echo [2/5] Installing requirements into current python...
python -m pip install -r tools\requirements.txt
if errorlevel 1 goto :end

echo.
echo [3/5] Verifying pyserial import...
python -c "import serial; print(serial.__version__)"
if errorlevel 1 goto :end

echo.
echo [4/5] Listing available serial ports...
python tools\serial_logger.py --list-ports
python -c "from serial.tools import list_ports; raise SystemExit(0 if list(list_ports.comports()) else 1)"
if errorlevel 1 (
    echo.
    echo No serial port detected. Please check USB-to-TTL adapter and Windows Device Manager.
    goto :end
)

echo.
set /p PORT_NAME=Enter the COM port to use, current test is COM5: 
if "%PORT_NAME%"=="" (
    set "PORT_NAME=COM5"
)
set /p TEST_LABEL=Enter test label, for example idle normal slight_unbalance severe_unbalance: 
if "%TEST_LABEL%"=="" (
    set "TEST_LABEL=unlabeled"
)

echo.
echo [5/5] Starting TinyPredict serial logger on %PORT_NAME% with label %TEST_LABEL%...
python tools\serial_logger.py --port %PORT_NAME% --baud 115200 --label %TEST_LABEL%

:end
echo.
pause
endlocal
