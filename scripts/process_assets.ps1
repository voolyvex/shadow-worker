# Main script for processing Shadow Worker RPG assets
param(
    [string]$ConfigPath = "scripts/asset_config.json",
    [switch]$Force,
    [switch]$SkipValidation,
    [switch]$NoBackup
)

# Import asset management module
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$modulePath = Join-Path $scriptPath "asset_management.ps1"
Import-Module $modulePath -Force

function Format-ByteSize {
    param([long]$Bytes)
    $sizes = 'Bytes', 'KB', 'MB', 'GB'
    $order = 0
    while ($Bytes -ge 1024 -and $order -lt $sizes.Count - 1) {
        $Bytes /= 1024
        $order++
    }
    return "{0:N2} {1}" -f $Bytes, $sizes[$order]
}

try {
    # Load configuration
    Write-Log "Loading configuration from $ConfigPath" -Type Info
    $config = Get-Content $ConfigPath -Raw | ConvertFrom-Json
    
    # Get initial stats if needed
    if (-not $Force) {
        Write-Log "Getting initial asset statistics..." -Type Info
        $initialStats = Get-AssetStats
    }
    
    # Initialize the asset management system
    $initParams = @{
        ProjectRoot = "."
    }
    if ($config.settings.createBackup -and -not $NoBackup) {
        $initParams.CreateBackup = $true
    }
    Initialize-AssetSystem @initParams
    
    Write-Log "Starting asset processing for Shadow Worker RPG v$($config.version)" -Type Info
    
    # Process each task
    foreach ($task in $config.tasks) {
        Write-Log "Processing task: $($task.description)" -Type Info
        
        switch ($task.type) {
            "extract" {
                $params = @{
                    ArchivePath = $task.source
                    DestinationPath = $task.destination
                    AssetType = $task.assetType
                    RemoveSource = $task.removeSource
                }
                if (-not $SkipValidation -and $task.validateContent) {
                    $params.ValidateContent = $true
                }
                Expand-GameArchive @params
            }
            "move" {
                $params = @{
                    SourcePath = $task.source
                    DestinationPath = $task.destination
                    AssetType = $task.assetType
                    CreateDirectory = $task.createDirectory
                }
                if ($task.backup -and -not $NoBackup) {
                    $params.Backup = $true
                }
                Move-GameAsset @params
            }
            default {
                Write-Log "Unknown task type: $($task.type)" -Type Warning
            }
        }
    }
    
    # Validate required directories
    Write-Log "Validating project structure..." -Type Info
    $missingDirs = @()
    foreach ($dir in $config.validation.requiredDirectories) {
        if (-not (Test-Path $dir)) {
            $missingDirs += $dir
        }
    }
    
    if ($missingDirs) {
        Write-Log "Warning: Missing required directories:" -Type Warning
        foreach ($dir in $missingDirs) {
            Write-Log "  - $dir" -Type Warning
        }
    }
    
    # Get final statistics
    Write-Log "Calculating final asset statistics..." -Type Info
    $finalStats = Get-AssetStats
    
    # Display summary
    Write-Log "`nAsset Processing Summary:" -Type Success
    foreach ($category in $finalStats.Keys) {
        $stats = $finalStats[$category]
        Write-Log "`n$category Assets:" -Type Info
        Write-Log "  Total Files: $($stats.TotalFiles)" -Type Info
        Write-Log "  Total Size: $(Format-ByteSize $stats.TotalSize)" -Type Info
        
        if ($stats.FileTypes.Count -gt 0) {
            Write-Log "  File Types:" -Type Info
            foreach ($ext in $stats.FileTypes.Keys) {
                Write-Log "    $ext : $($stats.FileTypes[$ext]) files" -Type Info
            }
        }
        
        if ($stats.Subdirectories.Count -gt 0) {
            Write-Log "  Files per Directory:" -Type Info
            foreach ($dir in $stats.Subdirectories.Keys) {
                Write-Log "    $dir : $($stats.Subdirectories[$dir]) files" -Type Info
            }
        }
        
        # Check total size against limits
        if ($config.validation.maxTotalSizeMB.$category) {
            $maxBytes = $config.validation.maxTotalSizeMB.$category * 1MB
            if ($stats.TotalSize -gt $maxBytes) {
                Write-Log "  Warning: Total size exceeds limit of $(Format-ByteSize $maxBytes)" -Type Warning
            }
        }
    }
    
    Write-Log "`nAsset processing completed successfully" -Type Success
}
catch {
    Write-Log "Asset processing failed: $_" -Type Error
    exit 1
} 