# Function to check Python installation
function Test-Python {
    try {
        $pythonVersion = python --version 2>&1
        if ($pythonVersion -match "Python 3\.[89]|Python 3\.[1-9][0-9]") {
            Write-Host "Using $pythonVersion"
            return $true
        } else {
            Write-Host "Python 3.8 or later is required."
            return $false
        }
    } catch {
        return $false
    }
}

# Function to provide installation instructions
function Show-PythonInstallInstructions {
    Write-Host "`nPython 3.8 or later is required but not found. Please follow these steps:`n"
    Write-Host "1. Visit https://www.python.org/downloads/"
    Write-Host "2. Download the latest Python 3 version for Windows"
    Write-Host "3. Run the installer"
    Write-Host "4. IMPORTANT: Check 'Add Python to PATH' during installation"
    Write-Host "5. Complete the installation"
    Write-Host "6. Restart your terminal"
    Write-Host "7. Run this script again`n"
}

# Check Python installation
if (!(Test-Python)) {
    Show-PythonInstallInstructions
    exit 1
}

# Create virtual environment if it doesn't exist
if (!(Test-Path ../.venv)) {
    Write-Host "`nCreating virtual environment..."
    python -m venv ../.venv
    if (!$?) {
        Write-Error "Failed to create virtual environment."
        exit 1
    }
}

# Activate virtual environment
Write-Host "`nActivating virtual environment..."
try {
    ../.venv/Scripts/Activate.ps1
} catch {
    Write-Error "Failed to activate virtual environment: $_"
    exit 1
}

# Install requirements
Write-Host "`nInstalling requirements..."
try {
    python -m pip install --upgrade pip
    python -m pip install -r requirements.txt
    if (!$?) {
        throw "Failed to install requirements"
    }
} catch {
    Write-Error "Failed to install requirements: $_"
    deactivate
    exit 1
}

# Run resource generation script
Write-Host "`nGenerating resources..."
try {
    python generate_resources.py
    if (!$?) {
        throw "Resource generation failed"
    }
} catch {
    Write-Error "Failed to generate resources: $_"
    deactivate
    exit 1
}

# Deactivate virtual environment
deactivate

Write-Host "`nResource generation complete!" 