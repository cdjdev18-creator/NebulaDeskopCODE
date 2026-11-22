@echo off
setlocal

if not exist "EXE" mkdir "EXE"

g++ -DUNICODE -D_UNICODE src\main.cpp -o EXE\NebulaDeskopCODE.exe -mwindows -luser32 -lgdi32 -lcomctl32 -lstdc++

if errorlevel 1 (
    echo Error de compilacion.
    exit /b 1
)

echo Compilacion exitosa: EXE\NebulaDeskopCODE.exe
pause
