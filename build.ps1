# Check if CMake is installed
$cmake = Get-Command "cmake" -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Error "CMake is not installed. Please install it from https://cmake.org/download/"
    exit 1
}

# Create build directory if it doesn't exist
$buildDir = "build"
if (-not (Test-Path $buildDir)) {
    Write-Host "Creating build directory..."
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Navigate to build directory
Push-Location $buildDir

try {
    # Configure CMake with detailed output
    Write-Host "`nConfiguring CMake..."
    $configureOutput = cmake .. -G "Visual Studio 17 2022" -A x64 2>&1
    Write-Host $configureOutput
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed with exit code $LASTEXITCODE"
    }

    # Build the project with detailed output
    Write-Host "`nBuilding project..."
    $buildOutput = cmake --build . --config Debug --verbose 2>&1
    Write-Host $buildOutput
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE"
    }

    Write-Host "`nBuild completed successfully!"
    Write-Host "You can find the executable in $buildDir\bin\Debug\"
} catch {
    Write-Error "Build failed: $_"
    Write-Host "Full error details:"
    Write-Host $_.Exception.Message
    Write-Host $_.ScriptStackTrace
    exit 1
} finally {
    # Return to original directory
    Pop-Location
} 