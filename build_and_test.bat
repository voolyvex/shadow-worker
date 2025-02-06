@echo off
setlocal EnableDelayedExpansion

set PROJECT_ROOT=%cd%
set BUILD_DIR=%PROJECT_ROOT%\build
set DUMP_DIR=%PROJECT_ROOT%\crash_dumps

if not exist "%DUMP_DIR%" mkdir "%DUMP_DIR%"

:: Set up Visual Studio environment
if not defined VSINSTALLDIR (
    echo Setting up Visual Studio environment...
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
)

:: Clear previous logs
set LOGFILE=%BUILD_DIR%\build_output.log
set ERRORLOG=%BUILD_DIR%\build_errors.log
if exist "%LOGFILE%" del /F /Q "%LOGFILE%" 2>nul
if exist "%ERRORLOG%" del /F /Q "%ERRORLOG%" 2>nul

:: Function to log messages
call :log "================================"
call :log "   Shadow Worker Build Pipeline"
call :log "================================"
call :log "Project root: %PROJECT_ROOT%"
call :log "Build directory: %BUILD_DIR%"
call :log "Build configuration: %BUILD_CONFIG%"
call :log "Crash dumps directory: %PROJECT_ROOT%\crash_dumps"

:: Verify project structure first
for %%d in (src include external tests) do (
    if not exist "%PROJECT_ROOT%\%%~d" (
        call :log "Error: Required directory '%%~d' not found!"
        exit /b 1
    )
)

if not exist "%PROJECT_ROOT%\CMakeLists.txt" (
    call :log "Error: CMakeLists.txt not found in root directory!"
    exit /b 1
)

:: Enhanced build directory cleaning
if exist "%BUILD_DIR%" (
    call :log "Cleaning build directory..."
    
    :: Kill any processes that might have files open
    call :log "Attempting to terminate processes using the build directory..."
    tasklist /fi "MODULES eq %BUILD_DIR%\*" /fo list > "%BUILD_DIR%\process_list.log" 2>&1
    call :log "Process list saved to %BUILD_DIR%\process_list.log"
    taskkill /F /FI "MODULES eq %BUILD_DIR%\*" >nul 2>&1
    taskkill /F /IM "sw_test_suite.exe" >nul 2>&1
    taskkill /F /IM "ShadowWorker.exe" >nul 2>&1
    taskkill /F /IM "cmake.exe" >nul 2>&1
    taskkill /F /IM "MSBuild.exe" >nul 2>&1
    
    :: Wait for processes to terminate
    timeout /t 2 /nobreak >nul
    
    :: Try to force close any remaining handles
    for /f "tokens=2" %%a in ('tasklist /fi "MODULES eq %BUILD_DIR%\*" /fo list ^| find "PID:"') do (
        taskkill /F /PID %%a >nul 2>&1
    )
    
    :: Remove read-only attributes and system attributes
    attrib -R -S -H "%BUILD_DIR%\*.*" /S /D >nul 2>&1
    
    :: Clear the directory contents first
    pushd "%BUILD_DIR%" 2>nul && (
        :: Delete all files including hidden ones
        del /F /S /Q /A:H *.* >nul 2>&1
        del /F /S /Q *.* >nul 2>&1
        
        :: Remove all subdirectories
        for /d %%i in (*) do rd /S /Q "%%i" >nul 2>&1
        
        popd
        
        :: Remove the build directory itself
        cd "%PROJECT_ROOT%"
        rd /S /Q "%BUILD_DIR%" >nul 2>&1
    )
    
    :: Final verification
    if exist "%BUILD_DIR%" (
        :: If still exists, try to force remove with robocopy
        mkdir "%BUILD_DIR%_empty" 2>nul
        robocopy "%BUILD_DIR%_empty" "%BUILD_DIR%" /MIR /NFL /NDL /NJH /NJS /NC /NS /NP >nul 2>&1
        rd /S /Q "%BUILD_DIR%_empty" >nul 2>&1
        rd /S /Q "%BUILD_DIR%" >nul 2>&1
    )
)

:: Create fresh build directory
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
) else (
    call :log "Error: Failed to clean build directory. Please close any applications using the directory and try again."
    exit /b 1
)

:: Change to build directory
cd "%BUILD_DIR%"

:: Configure CMake with enhanced debug settings
if not defined BUILD_CONFIG set BUILD_CONFIG=Debug
cmake -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=%BUILD_CONFIG% ^
    -DCMAKE_SYSTEM_VERSION="10.0" ^
    -DCMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION="10.0.22621.0" ^
    -DCMAKE_VS_JUST_MY_CODE_DEBUGGING=ON ^
    -DCMAKE_CXX_FLAGS_DEBUG="/MTd /Zi /Ob0 /Od /RTC1 /GS /sdl /W4 /WX /D_DEBUG /DDEBUG_MEMORY_TRACKING" ^
    -DCMAKE_C_FLAGS_DEBUG="/MTd /Zi /Ob0 /Od /RTC1 /GS /sdl /W4 /WX /D_DEBUG /DDEBUG_MEMORY_TRACKING" ^
    -DCMAKE_EXE_LINKER_FLAGS_DEBUG="/DEBUG:FULL /INCREMENTAL:NO" ^
    ..

:: Build project with detailed output
cmake --build . --config %BUILD_CONFIG% --verbose

:: Run tests with enhanced error reporting
set CTEST_OUTPUT_ON_FAILURE=1
set CTEST_PARALLEL_LEVEL=1
ctest -C %BUILD_CONFIG% --output-on-failure --verbose --timeout 300

:: Check for crash dumps
if exist "%PROJECT_ROOT%\crash_dumps\*" (
    call :log "Crash dumps found in %PROJECT_ROOT%\crash_dumps"
    dir "%PROJECT_ROOT%\crash_dumps"
)

:: Return to project root
cd "%PROJECT_ROOT%"

call :log "Build pipeline completed!"
exit /b 0

:log
echo %~1
if exist "%LOGFILE%" (
    echo %~1 >> "%LOGFILE%"
)
goto :eof 