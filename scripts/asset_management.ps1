# Asset Management Module for Shadow Worker
# This module provides functions for managing game assets

# Module variables
$script:assetRoot = Join-Path -Path $PSScriptRoot -ChildPath ".." | Join-Path -ChildPath "art"
$script:backupRoot = Join-Path -Path $PSScriptRoot -ChildPath ".." | Join-Path -ChildPath "backup"
$script:logFile = Join-Path -Path $PSScriptRoot -ChildPath ".." | Join-Path -ChildPath "logs" | Join-Path -ChildPath "asset_management.log"

# Ensure log directory exists
$logDir = Split-Path -Path $script:logFile -Parent
if (-not (Test-Path -Path $logDir)) {
    New-Item -ItemType Directory -Force -Path $logDir | Out-Null
}

function Write-Log {
    param(
        [Parameter(Mandatory=$true)]
        [string]$Message,
        [ValidateSet('Info','Warning','Error','Success')]
        [string]$Level = 'Info'
    )
    
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    Add-Content -Path $script:logFile -Value $logMessage
    
    switch ($Level) {
        'Warning' { Write-Host $Message -ForegroundColor Yellow }
        'Error' { Write-Host $Message -ForegroundColor Red }
        'Success' { Write-Host $Message -ForegroundColor Green }
        default { Write-Host $Message }
    }
}

function Initialize-AssetSystem {
    [CmdletBinding()]
    param(
        [switch]$CreateBackup
    )
    
    try {
        # Create asset directory structure if it doesn't exist
        $directories = @(
            "sprites/characters/player",
            "sprites/characters/enemies",
            "sprites/characters/npcs",
            "sprites/environment/tiles",
            "sprites/environment/props",
            "sprites/effects",
            "sprites/ui",
            "audio/music",
            "audio/sfx",
            "templates/rooms",
            "templates/corridors"
        )
        
        foreach ($dir in $directories) {
            $path = Join-Path -Path $script:assetRoot -ChildPath $dir
            if (-not (Test-Path -Path $path)) {
                New-Item -ItemType Directory -Force -Path $path | Out-Null
            }
        }
        
        if ($CreateBackup) {
            $date = Get-Date -Format "yyyy-MM-dd"
            $backupDir = Join-Path -Path $script:backupRoot -ChildPath $date
            
            Write-Log "Created backup directory: backup\$date" -Level Info
            
            if (Test-Path -Path $script:assetRoot) {
                Copy-Item -Path $script:assetRoot -Destination $backupDir -Recurse -Force
                Write-Log "Created backup of art directory" -Level Success
            }
        }
        
        Write-Log "Asset management system initialized successfully" -Level Success
    }
    catch {
        Write-Log "Failed to initialize asset system: $_" -Level Error
        throw
    }
}

function Import-GameAssets {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true)]
        [string]$SourcePath,
        [Parameter(Mandatory=$true)]
        [string]$Category,
        [string[]]$AllowedExtensions = @('.png','.jpg','.wav','.ogg','.mp3')
    )
    
    try {
        $targetDir = Join-Path -Path $script:assetRoot -ChildPath $Category
        
        if (-not (Test-Path -Path $targetDir)) {
            New-Item -ItemType Directory -Force -Path $targetDir | Out-Null
        }
        
        if (Test-Path -Path $SourcePath) {
            Get-ChildItem -Path $SourcePath -Recurse -File | 
            Where-Object { $AllowedExtensions -contains $_.Extension } |
            ForEach-Object {
                $targetPath = Join-Path -Path $targetDir -ChildPath $_.Name
                Copy-Item -Path $_.FullName -Destination $targetPath -Force
                Write-Log "Imported: $($_.Name) to $Category" -Level Success
            }
        }
        else {
            Write-Log "Source path not found: $SourcePath" -Level Warning
        }
    }
    catch {
        Write-Log "Failed to import assets: $_" -Level Error
        throw
    }
}

function Remove-UnusedAssets {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true)]
        [string]$Category,
        [string[]]$ExcludePatterns
    )
    
    try {
        $assetDir = Join-Path -Path $script:assetRoot -ChildPath $Category
        
        if (Test-Path -Path $assetDir) {
            Get-ChildItem -Path $assetDir -Recurse -File |
            Where-Object {
                $file = $_
                -not ($ExcludePatterns | Where-Object { $file.Name -like $_ })
            } |
            ForEach-Object {
                Remove-Item -Path $_.FullName -Force
                Write-Log "Removed unused asset: $($_.Name)" -Level Info
            }
        }
    }
    catch {
        Write-Log "Failed to remove unused assets: $_" -Level Error
        throw
    }
}

# Export module members
Export-ModuleMember -Function Initialize-AssetSystem, Import-GameAssets, Remove-UnusedAssets 