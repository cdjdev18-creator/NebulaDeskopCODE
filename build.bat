@echo off
setlocal

:: Añadir MinGW-w64 al PATH si no está presente
where g++ >nul 2>&1
if errorlevel 1 set PATH=C:\msys64\mingw64\bin;%PATH%

if not exist "EXE" mkdir "EXE"
if not exist "EXE\ui" mkdir "EXE\ui"
if not exist "EXE\confis" mkdir "EXE\confis"

:: Ensure WebView2 loader DLL is alongside the executable (required at runtime)
if not exist "EXE\WebView2Loader.dll" copy "lib\WebView2Loader.dll" "EXE\WebView2Loader.dll" >nul 2>&1

:: Copy UI resources for distribution
copy "ui\index.html" "EXE\ui\index.html" >nul
copy "ui\style.css" "EXE\ui\style.css" >nul
copy "ui\script.js" "EXE\ui\script.js" >nul
copy "ui\settings.html" "EXE\ui\settings.html" >nul
copy "ui\agentmanager.html" "EXE\ui\agentmanager.html" >nul

:: Copy default configuration
if exist "confis\confiUI.txt" copy "confis\confiUI.txt" "EXE\confis\confiUI.txt" >nul

:: Compile with WebView2 support (MinGW-w64)
g++ -DUNICODE -D_UNICODE -Iinclude src\main.cpp -o EXE\NebulaDeskopCODE.exe -mwindows -luser32 -lgdi32 -lcomctl32 -lshlwapi -lole32 -loleaut32 -lWebView2Loader -lcrypt32 -lshell32 -Llib -lstdc++

if errorlevel 1 (
    echo Error de compilacion.
    exit /b 1
)

echo Compilacion exitosa: EXE\NebulaDeskopCODE.exe
echo Recursos: EXE\ui\, EXE\confis\, EXE\WebView2Loader.dll
pause
