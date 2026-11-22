@echo off
setlocal

if not exist "EXE" mkdir "EXE"
if not exist "EXE\ui" mkdir "EXE\ui"
if not exist "EXE\confis" mkdir "EXE\confis"

:: Ensure WebView2 loader DLL is alongside the executable (required at runtime)
if not exist "EXE\WebView2Loader.dll" copy "EXE\WebView2Loader.dll" "EXE\WebView2Loader.dll" >nul 2>&1
if not exist "EXE\WebView2Loader.dll" copy "lib\WebView2Loader.dll" "EXE\WebView2Loader.dll" >nul 2>&1

:: Copy UI resources for distribution
copy "ui\index.html" "EXE\ui\index.html" >nul
copy "ui\style.css" "EXE\ui\style.css" >nul
copy "ui\script.js" "EXE\ui\script.js" >nul

:: Copy default configuration
if exist "confis\confiUI.txt" copy "confis\confiUI.txt" "EXE\confis\confiUI.txt" >nul

:: Compile with WebView2 support (MinGW-w64)
g++ -DUNICODE -D_UNICODE -Iinclude src\main.cpp -o EXE\NebulaDeskopCODE.exe -mwindows -luser32 -lgdi32 -lcomctl32 -lshlwapi -lole32 -loleaut32 -lWebView2Loader -Llib -lstdc++

if errorlevel 1 (
    echo Error de compilacion.
    exit /b 1
)

echo Compilacion exitosa: EXE\NebulaDeskopCODE.exe
echo Recursos: EXE\ui\, EXE\confis\, EXE\WebView2Loader.dll
pause
