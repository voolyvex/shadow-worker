@echo off
setlocal EnableDelayedExpansion

:: Set up Visual Studio environment
if not defined VSINSTALLDIR (
    echo Setting up Visual Studio environment...
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
)

:: Unified build directory
set BUILD_DIR=%cd%\build
set BUILD_CONFIG=Debug

:: Clean build directory
if exist "%BUILD_DIR%" (
    echo Cleaning build directory...
    rmdir /S /Q "%BUILD_DIR%"
)

:: Create build directory
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

:: Configure CMake
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_CONFIG% ..

:: Build project
cmake --build . --config %BUILD_CONFIG%

:: Run tests
ctest -C %BUILD_CONFIG% --output-on-failure

:: Store original directory and set paths
set SOURCE_DIR=%cd%
set SHADOW_WORKSPACE=%SOURCE_DIR%\shadow_workspace
set TEST_CONFIG=Debug
set LOGFILE=%SOURCE_DIR%\build_output.log
set ERRORLOG=%SOURCE_DIR%\build_errors.log

:: Clear previous logs
if exist %LOGFILE% del %LOGFILE%
if exist %ERRORLOG% del %ERRORLOG%

:: Function to log messages
call :log "================================"
call :log "   Shadow Worker Build Pipeline"
call :log "================================"
call :log "Source directory: %SOURCE_DIR%"
call :log "Shadow workspace: %SHADOW_WORKSPACE%"
call :log "Build directory: %BUILD_DIR%"
call :log "Build configuration: %BUILD_CONFIG%"

:: Verify required directories and files
set MISSING_DEPS=0
for %%d in (src include external) do (
    if not exist "%%d" (
        call :log "Error: Required directory '%%d' not found!"
        set /A MISSING_DEPS+=1
    )
)

if not exist "CMakeLists.txt" (
    call :log "Error: CMakeLists.txt not found!"
    set /A MISSING_DEPS+=1
)

if %MISSING_DEPS% GTR 0 (
    call :log "Error: Missing required dependencies. Build cannot continue."
    exit /b 1
)

:: Clean shadow workspace with improved error handling
if exist "%SHADOW_WORKSPACE%" (
    call :log "Cleaning shadow workspace..."
    
    :: Try to remove read-only attributes first
    attrib -R "%SHADOW_WORKSPACE%\*.*" /S /D > nul 2>&1
    
    :: Wait a bit to ensure any file handles are released
    timeout /t 1 /nobreak > nul
    
    :: Try to delete the directory
    rmdir /S /Q "%SHADOW_WORKSPACE%" 2> temp_error.txt
    if errorlevel 1 (
        set /p ERROR_MSG=<temp_error.txt
        call :log "Warning: Initial cleanup failed. Retrying with fallback method..."
        
        :: Fallback: try to delete files first, then directory
        del /F /S /Q "%SHADOW_WORKSPACE%\*.*" > nul 2>&1
        timeout /t 1 /nobreak > nul
        rmdir /S /Q "%SHADOW_WORKSPACE%" 2> temp_error.txt
        
        if errorlevel 1 (
            set /p ERROR_MSG=<temp_error.txt
            call :log "Error: Failed to clean shadow workspace. Error: !ERROR_MSG!"
            del temp_error.txt
            exit /b 1
        )
    )
    del temp_error.txt 2> nul
)

:: Create shadow workspace with proper error handling
call :log "Creating shadow workspace..."
mkdir "%SHADOW_WORKSPACE%" 2> temp_error.txt
if errorlevel 1 (
    set /p ERROR_MSG=<temp_error.txt
    call :log "Error: Failed to create shadow workspace. Error: !ERROR_MSG!"
    del temp_error.txt
    exit /b 1
)
del temp_error.txt 2> nul

:: Copy project directories with verification
for %%d in (src include external tests cmake assets) do (
    if exist "%%d" (
        call :log "Copying %%d directory..."
        mkdir "%SHADOW_WORKSPACE%\%%d" 2>nul
        xcopy /E /I /Y "%%d" "%SHADOW_WORKSPACE%\%%d" >> %LOGFILE% 2>> %ERRORLOG%
        if errorlevel 1 (
            call :log "Error: Failed to copy %%d directory"
            cd "%SOURCE_DIR%"
            exit /b 1
        )
    ) else (
        call :log "Note: Optional directory %%d not found, skipping..."
    )
)

:: Copy CMake files with verification
if exist "CMakeLists.txt" (
    call :log "Copying CMakeLists.txt..."
    copy /Y "CMakeLists.txt" "%SHADOW_WORKSPACE%\" >> %LOGFILE% 2>> %ERRORLOG%
    if errorlevel 1 (
        call :log "Error: Failed to copy CMakeLists.txt"
        cd "%SOURCE_DIR%"
        exit /b 1
    )
)

:: Run tests with detailed output
call :log "Running tests in %TEST_CONFIG% configuration..."
ctest -C %TEST_CONFIG% --output-on-failure > test_output.txt 2>&1
if errorlevel 1 (
    call :log "Error: Tests failed"
    type test_output.txt >> %ERRORLOG%
    type test_output.txt
    cd "%SOURCE_DIR%"
    exit /b 1
)

:: Return to source directory
cd "%SOURCE_DIR%"

call :log "Build pipeline completed successfully!"
exit /b 0

:log
echo %~1
echo %~1 >> %LOGFILE%
goto :eof 