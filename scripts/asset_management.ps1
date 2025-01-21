# Asset Management System for Shadow Worker RPG
# Implements robust error handling and logging for asset operations

# Configuration
$script:Config = @{
    LogPath = "logs/$(Get-Date -Format 'yyyy-MM-dd')/session.log"
    ErrorLogPath = "logs/$(Get-Date -Format 'yyyy-MM-dd')/errors.log"
    AssetStructure = @{
        Sprites = @{
            Path = "art/sprites"
            Subdirs = @(
                "characters",
                "environment/indoor",
                "environment/outdoor",
                "environment/props",
                "effects/particles",
                "effects/weather",
                "effects/lighting",
                "ui"
            )
            AllowedExtensions = @(".png", ".jpg", ".gif")
            MaxFileSizeMB = 10
            Validation = @{
                RequiredDimensions = @{
                    "characters" = @(32, 32)
                    "environment" = @(32, 32)
                    "props" = @(32, 32)
                }
                RequiredFormats = @{
                    "characters" = "RGBA"
                    "effects" = "RGBA"
                }
            }
        }
        Audio = @{
            Path = "art/audio"
            Subdirs = @(
                "music/ambient",
                "music/combat",
                "music/boss",
                "sfx/environment",
                "sfx/interaction",
                "sfx/combat",
                "dialogue",
                "ambient/indoor",
                "ambient/outdoor",
                "ambient/weather"
            )
            AllowedExtensions = @(".ogg", ".wav", ".mp3")
            MaxFileSizeMB = 50
            Validation = @{
                RequiredFormats = @{
                    "music" = "OGG"
                    "sfx" = "WAV"
                    "ambient" = "OGG"
                }
                MaxDuration = @{
                    "sfx" = 10
                    "ambient" = 60
                    "music" = 300
                }
            }
        }
        Maps = @{
            Path = "art/maps"
            Subdirs = @(
                "templates/rooms",
                "templates/corridors",
                "templates/outdoor",
                "tilesets/indoor",
                "tilesets/outdoor",
                "tilesets/props",
                "backgrounds/indoor",
                "backgrounds/outdoor"
            )
            AllowedExtensions = @(".png", ".jpg", ".tmx", ".json")
            MaxFileSizeMB = 25
            Validation = @{
                RequiredProperties = @{
                    "rooms" = @("walls", "floor", "spawns", "doors")
                    "corridors" = @("connections", "variants")
                    "outdoor" = @("biome", "features", "transitions")
                }
                TileSize = 32
                MinRoomSize = 6
                MaxRoomSize = 20
            }
        }
        Templates = @{
            Path = "art/templates"
            Subdirs = @(
                "rooms/combat",
                "rooms/treasure",
                "rooms/shop",
                "rooms/boss",
                "outdoor/biomes",
                "outdoor/features",
                "outdoor/transitions"
            )
            AllowedExtensions = @(".json", ".tscn")
            MaxFileSizeMB = 1
            Validation = @{
                RequiredFields = @{
                    "rooms" = @("type", "difficulty", "connections", "spawns")
                    "biomes" = @("type", "features", "transitions", "weather")
                }
            }
        }
        Docs = @{
            Path = "art/docs"
            Subdirs = @("licenses", "specifications", "templates")
            AllowedExtensions = @(".txt", ".md", ".pdf")
            MaxFileSizeMB = 10
        }
    }
    BackupPath = "backup/$(Get-Date -Format 'yyyy-MM-dd')"
    ChunkSize = 32  # Size of chunks for outdoor areas
    MaxLoadedChunks = 9  # Maximum number of chunks to keep in memory
    ResourcePooling = @{
        MaxPoolSize = 100  # Maximum number of pooled objects
        PooledTypes = @("props", "effects", "npcs")
    }
}

# Template scaling configuration
$script:TemplateConfig = @{
    ScaleModes = @{
        PixelPerfect = @{
            Filter = "Nearest"
            EnableMipmaps = $false
        }
        Smooth = @{
            Filter = "Linear"
            EnableMipmaps = $true
        }
        Detailed = @{
            Filter = "Anisotropic"
            EnableMipmaps = $true
        }
    }
    Sizes = @{
        tiny = @(4, 4)
        small = @(6, 6)
        medium = @(8, 8)
        large = @(12, 12)
        boss = @(16, 16)
    }
}

# Initialize logging
function Initialize-AssetSystem {
    param(
        [string]$ProjectRoot = ".",
        [switch]$CreateBackup
    )
    
    try {
        # Create log directories
        New-Item -ItemType Directory -Force -Path (Split-Path $script:Config.LogPath) | Out-Null
        New-Item -ItemType Directory -Force -Path (Split-Path $script:Config.ErrorLogPath) | Out-Null
        
        # Create backup if requested
        if ($CreateBackup) {
            New-Item -ItemType Directory -Force -Path $script:Config.BackupPath | Out-Null
            Write-Log "Created backup directory: $($script:Config.BackupPath)" -Type Info
            
            # Backup existing art directory if it exists
            if (Test-Path "$ProjectRoot/art") {
                Copy-Item -Path "$ProjectRoot/art" -Destination $script:Config.BackupPath -Recurse
                Write-Log "Created backup of art directory" -Type Success
            }
        }
        
        # Create asset structure
        foreach ($category in $script:Config.AssetStructure.Keys) {
            $categoryConfig = $script:Config.AssetStructure[$category]
            $basePath = Join-Path $ProjectRoot $categoryConfig.Path
            
            # Create base directory
            New-Item -ItemType Directory -Force -Path $basePath | Out-Null
            
            # Create subdirectories
            foreach ($subdir in $categoryConfig.Subdirs) {
                $subdirPath = Join-Path $basePath $subdir
                New-Item -ItemType Directory -Force -Path $subdirPath | Out-Null
            }
        }
        
        Write-Log "Asset management system initialized successfully" -Type Success
    }
    catch {
        Write-Log "Failed to initialize asset system: $_" -Type Error
        throw
    }
}

# Enhanced logging function with debug level
function Write-Log {
    param(
        [string]$Message,
        [ValidateSet('Debug', 'Info', 'Success', 'Warning', 'Error')]
        [string]$Type = 'Info',
        [switch]$NoConsole
    )
    
    $timestamp = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'
    $formattedMessage = "[$timestamp] [$Type] $Message"
    
    # Console output with color
    if (-not $NoConsole) {
        $colors = @{
            Debug = 'Gray'
            Info = 'White'
            Success = 'Green'
            Warning = 'Yellow'
            Error = 'Red'
        }
        Write-Host $formattedMessage -ForegroundColor $colors[$Type]
    }
    
    # File logging
    $logPath = if ($Type -eq 'Error') { $script:Config.ErrorLogPath } else { $script:Config.LogPath }
    Add-Content -Path $logPath -Value $formattedMessage
}

# Asset validation function
function Test-GameAsset {
    param(
        [Parameter(Mandatory=$true)]
        [string]$FilePath,
        [Parameter(Mandatory=$true)]
        [string]$AssetType
    )
    
    try {
        $file = Get-Item $FilePath
        $categoryConfig = $script:Config.AssetStructure[$AssetType]
        
        # Check file exists
        if (-not $file) {
            throw "File not found: $FilePath"
        }
        
        # Check extension
        $extension = [System.IO.Path]::GetExtension($FilePath).ToLower()
        if (-not $categoryConfig.AllowedExtensions.Contains($extension)) {
            throw "Invalid file extension: $extension. Allowed: $($categoryConfig.AllowedExtensions -join ', ')"
        }
        
        # Check file size
        $fileSizeMB = $file.Length / 1MB
        if ($fileSizeMB -gt $categoryConfig.MaxFileSizeMB) {
            throw "File size ($fileSizeMB MB) exceeds maximum allowed size ($($categoryConfig.MaxFileSizeMB) MB)"
        }
        
        # Additional checks based on file type
        switch ($extension) {
            ".png" {
                # TODO: Add image dimension validation
                Write-Log "PNG validation not yet implemented" -Type Debug -NoConsole
            }
            ".ogg" {
                # TODO: Add audio format validation
                Write-Log "OGG validation not yet implemented" -Type Debug -NoConsole
            }
        }
        
        return $true
    }
    catch {
        Write-Log "Asset validation failed: $_" -Type Error
        return $false
    }
}

# Enhanced archive extraction with validation
function Expand-GameArchive {
    param(
        [Parameter(Mandatory=$true)]
        [string]$ArchivePath,
        [Parameter(Mandatory=$true)]
        [string]$DestinationPath,
        [Parameter(Mandatory=$true)]
        [string]$AssetType,
        [switch]$RemoveSource,
        [switch]$ValidateContent
    )
    
    try {
        # Validate archive exists
        if (-not (Test-Path $ArchivePath)) {
            throw "Archive not found: $ArchivePath"
        }
        
        # Create destination if needed
        New-Item -ItemType Directory -Force -Path $DestinationPath | Out-Null
        
        # Create temp extraction directory
        $tempPath = Join-Path $env:TEMP "GameAssetExtraction_$(Get-Random)"
        New-Item -ItemType Directory -Force -Path $tempPath | Out-Null
        
        # Extract based on extension
        $extension = [System.IO.Path]::GetExtension($ArchivePath)
        switch ($extension.ToLower()) {
            ".zip" {
                Expand-Archive -Path $ArchivePath -DestinationPath $tempPath -Force
            }
            ".rar" {
                & 7z x $ArchivePath "-o$tempPath" -y
                if ($LASTEXITCODE -ne 0) {
                    throw "7-Zip extraction failed with code: $LASTEXITCODE"
                }
            }
            default {
                throw "Unsupported archive type: $extension"
            }
        }
        
        # Validate extracted content if requested
        if ($ValidateContent) {
            $validFiles = 0
            $invalidFiles = 0
            Get-ChildItem -Path $tempPath -Recurse -File | ForEach-Object {
                if (Test-GameAsset -FilePath $_.FullName -AssetType $AssetType) {
                    $validFiles++
                } else {
                    $invalidFiles++
                    Write-Log "Invalid asset found: $($_.Name)" -Type Warning
                }
            }
            Write-Log "Validation complete: $validFiles valid files, $invalidFiles invalid files" -Type Info
        }
        
        # Move validated content to final destination
        Move-Item -Path "$tempPath/*" -Destination $DestinationPath -Force
        Remove-Item -Path $tempPath -Recurse -Force
        
        Write-Log "Successfully extracted: $ArchivePath" -Type Success
        
        if ($RemoveSource) {
            Remove-Item -Path $ArchivePath -Force
            Write-Log "Removed source archive: $ArchivePath" -Type Info
        }
    }
    catch {
        Write-Log "Failed to extract archive $ArchivePath : $_" -Type Error
        throw
    }
}

# Enhanced file moving with validation and backup
function Move-GameAsset {
    param(
        [Parameter(Mandatory=$true)]
        [string]$SourcePath,
        [Parameter(Mandatory=$true)]
        [string]$DestinationPath,
        [Parameter(Mandatory=$true)]
        [string]$AssetType,
        [switch]$CreateDirectory,
        [switch]$Backup
    )
    
    try {
        if ($CreateDirectory) {
            New-Item -ItemType Directory -Force -Path $DestinationPath | Out-Null
        }
        
        # Validate source exists
        if (-not (Test-Path $SourcePath)) {
            throw "Source path not found: $SourcePath"
        }
        
        # Get files to move
        $files = Get-Item -Path $SourcePath
        
        foreach ($file in $files) {
            # Validate file
            if (-not (Test-GameAsset -FilePath $file.FullName -AssetType $AssetType)) {
                Write-Log "Skipping invalid asset: $($file.Name)" -Type Warning
                continue
            }
            
            # Create backup if requested
            if ($Backup) {
                $backupPath = Join-Path $script:Config.BackupPath $file.Name
                Copy-Item -Path $file.FullName -Destination $backupPath -Force
                Write-Log "Created backup: $backupPath" -Type Info
            }
            
            # Move file
            $destinationFile = Join-Path $DestinationPath $file.Name
            Move-Item -Path $file.FullName -Destination $destinationFile -Force
            Write-Log "Moved: $($file.Name) → $destinationFile" -Type Success
        }
    }
    catch {
        Write-Log "Failed to move asset $SourcePath : $_" -Type Error
        throw
    }
}

# Get asset statistics
function Get-AssetStats {
    param(
        [string]$ProjectRoot = "."
    )
    
    try {
        $stats = @{}
        
        foreach ($category in $script:Config.AssetStructure.Keys) {
            $categoryConfig = $script:Config.AssetStructure[$category]
            $basePath = Join-Path $ProjectRoot $categoryConfig.Path
            
            $stats[$category] = @{
                TotalFiles = 0
                TotalSize = 0
                FileTypes = @{}
                Subdirectories = @{}
            }
            
            if (Test-Path $basePath) {
                $files = Get-ChildItem -Path $basePath -Recurse -File
                
                $stats[$category].TotalFiles = $files.Count
                $stats[$category].TotalSize = ($files | Measure-Object -Property Length -Sum).Sum
                
                # Count file types
                $files | ForEach-Object {
                    $ext = $_.Extension.ToLower()
                    if (-not $stats[$category].FileTypes[$ext]) {
                        $stats[$category].FileTypes[$ext] = 0
                    }
                    $stats[$category].FileTypes[$ext]++
                }
                
                # Count files per subdirectory
                foreach ($subdir in $categoryConfig.Subdirs) {
                    $subdirPath = Join-Path $basePath $subdir
                    if (Test-Path $subdirPath) {
                        $stats[$category].Subdirectories[$subdir] = (Get-ChildItem -Path $subdirPath -File).Count
                    }
                }
            }
        }
        
        return $stats
    }
    catch {
        Write-Log "Failed to get asset statistics: $_" -Type Error
        throw
    }
}

# Add new function for template validation
function Test-LevelTemplate {
    param(
        [Parameter(Mandatory=$true)]
        [string]$TemplatePath,
        [Parameter(Mandatory=$true)]
        [string]$TemplateType
    )
    
    try {
        $template = Get-Content $TemplatePath -Raw | ConvertFrom-Json
        $validation = $script:Config.AssetStructure.Templates.Validation
        
        # Validate required fields
        $requiredFields = $validation.RequiredFields[$TemplateType]
        foreach ($field in $requiredFields) {
            if (-not $template.$field) {
                throw "Missing required field: $field"
            }
        }
        
        # Validate specific template types
        switch ($TemplateType) {
            "rooms" {
                if ($template.size.x -lt $validation.MinRoomSize -or 
                    $template.size.y -lt $validation.MinRoomSize) {
                    throw "Room size below minimum: $($validation.MinRoomSize)"
                }
                if ($template.size.x -gt $validation.MaxRoomSize -or 
                    $template.size.y -gt $validation.MaxRoomSize) {
                    throw "Room size exceeds maximum: $($validation.MaxRoomSize)"
                }
            }
            "biomes" {
                if (-not $template.features -or $template.features.Count -eq 0) {
                    throw "Biome must define at least one feature"
                }
            }
        }
        
        return $true
    }
    catch {
        Write-Log "Template validation failed: $_" -Type Error
        return $false
    }
}

# Add new function for chunk management
function Get-AssetChunk {
    param(
        [Parameter(Mandatory=$true)]
        [int]$ChunkX,
        [Parameter(Mandatory=$true)]
        [int]$ChunkY
    )
    
    try {
        $chunkSize = $script:Config.ChunkSize
        $chunkPath = "art/chunks/chunk_${ChunkX}_${ChunkY}.json"
        
        if (Test-Path $chunkPath) {
            $chunk = Get-Content $chunkPath -Raw | ConvertFrom-Json
            
            # Validate chunk data
            if (-not $chunk.tiles -or -not $chunk.props -or -not $chunk.entities) {
                throw "Invalid chunk data structure"
            }
            
            return $chunk
        }
        else {
            # Generate new chunk
            $chunk = @{
                position = @{
                    x = $ChunkX * $chunkSize
                    y = $ChunkY * $chunkSize
                }
                size = $chunkSize
                tiles = @()
                props = @()
                entities = @()
            }
            
            # Save chunk
            $chunk | ConvertTo-Json -Depth 10 | Set-Content $chunkPath
            
            return $chunk
        }
    }
    catch {
        Write-Log "Failed to load/generate chunk ($ChunkX, $ChunkY): $_" -Type Error
        throw
    }
}

# Add new function for resource pooling
function Get-PooledResource {
    param(
        [Parameter(Mandatory=$true)]
        [string]$ResourceType,
        [string]$Variant = "default"
    )
    
    try {
        $poolConfig = $script:Config.ResourcePooling
        
        if (-not $poolConfig.PooledTypes.Contains($ResourceType)) {
            throw "Resource type not configured for pooling: $ResourceType"
        }
        
        # TODO: Implement actual pooling logic
        Write-Log "Resource pooling not yet implemented" -Type Debug -NoConsole
        
        return $null
    }
    catch {
        Write-Log "Failed to get pooled resource: $_" -Type Error
        throw
    }
}

# Add template scaling and integration functions

function Test-TemplateCompatibility {
    param(
        [Parameter(Mandatory=$true)]
        [string]$TemplatePath,
        [Parameter(Mandatory=$true)]
        [string]$TargetSize
    )
    
    try {
        $template = Get-Content $TemplatePath | ConvertFrom-Json -AsHashtable
        
        if (-not $script:TemplateConfig.Sizes.ContainsKey($TargetSize)) {
            Write-Log "Invalid target size: $TargetSize" -Level "Error"
            return $false
        }
        
        $targetDimensions = $script:TemplateConfig.Sizes[$TargetSize]
        if ($template.size.x -gt $targetDimensions[0] -or $template.size.y -gt $targetDimensions[1]) {
            Write-Log "Room size exceeds maximum: $($template.size.x)x$($template.size.y) > $($targetDimensions[0])x$($targetDimensions[1])" -Level "Error"
            return $false
        }
        
        return $true
    }
    catch {
        Write-Log "Error checking template compatibility: $_" -Level "Error"
        return $false
    }
}

function Get-ScaledTemplate {
    param(
        [string]$TemplatePath,
        [string]$TargetSize,
        [string]$ScaleMode = "PixelPerfect"
    )
    
    if (-not (Test-TemplateCompatibility -TemplatePath $TemplatePath -TargetSize $TargetSize)) {
        Write-Log "Template incompatible: Invalid target size: $TargetSize" -Level "Error"
        throw "Template incompatible: Invalid target size: $TargetSize"
    }
    
    try {
        $template = Get-Content $TemplatePath | ConvertFrom-Json -AsHashtable
        $targetDimensions = $script:TemplateConfig.Sizes[$TargetSize]
        
        # Scale the template
        $template.size.x = $targetDimensions[0]
        $template.size.y = $targetDimensions[1]
        
        # Apply scaling mode settings
        $template.properties.scaling = $script:TemplateConfig.ScaleModes[$ScaleMode]
        
        return $template
    }
    catch {
        Write-Log "Failed to scale template: $_" -Level "Error"
        throw "Failed to scale template: $_"
    }
}

function New-TemplateTransition {
    param(
        [string]$SourcePath,
        [string]$TargetPath,
        [string]$TransitionType = "Smooth"
    )
    
    try {
        $sourceTiles = (Get-Content $SourcePath | ConvertFrom-Json).tiles
        $targetTiles = (Get-Content $TargetPath | ConvertFrom-Json).tiles
        
        $transitionTiles = switch ($TransitionType) {
            "Smooth" { Get-BlendedTiles -SourceTiles $sourceTiles -TargetTiles $targetTiles }
            "Sharp" { Get-DirectTransitionTiles -SourceTiles $sourceTiles -TargetTiles $targetTiles }
            default { throw "Unsupported transition type: $TransitionType" }
        }
        
        return @{
            type = "transition"
            sourceTemplate = $SourcePath
            targetTemplate = $TargetPath
            transitionType = $TransitionType
            tiles = $transitionTiles
        }
    }
    catch {
        Write-Log "Failed to create template transition: $_" -Level "Error"
        throw "Failed to create template transition: $_"
    }
}

function Get-BlendedTiles {
    param(
        [hashtable]$SourceTiles,
        [hashtable]$TargetTiles,
        [float]$BlendFactor = 0.5
    )
    
    $blendedTiles = @{}
    foreach ($key in $SourceTiles.Keys) {
        if ($TargetTiles.ContainsKey($key)) {
            $blendedTiles[$key] = @{
                Type = $SourceTiles[$key].Type
                Properties = @{}
            }
            foreach ($prop in $SourceTiles[$key].Properties.Keys) {
                if ($TargetTiles[$key].Properties.ContainsKey($prop)) {
                    $sourceVal = $SourceTiles[$key].Properties[$prop]
                    $targetVal = $TargetTiles[$key].Properties[$prop]
                    if ($sourceVal -is [int] -or $sourceVal -is [float]) {
                        $blendedTiles[$key].Properties[$prop] = $sourceVal * (1 - $BlendFactor) + $targetVal * $BlendFactor
                    } else {
                        $blendedTiles[$key].Properties[$prop] = if ($BlendFactor -ge 0.5) { $targetVal } else { $sourceVal }
                    }
                }
            }
        }
    }
    return $blendedTiles
}

function Get-DirectTransitionTiles {
    param(
        [hashtable]$SourceTiles,
        [hashtable]$TargetTiles
    )
    
    $transitionTiles = @{}
    foreach ($key in $SourceTiles.Keys) {
        if ($TargetTiles.ContainsKey($key)) {
            $transitionTiles[$key] = @{
                Type = "Transition"
                Properties = @{
                    SourceTile = $SourceTiles[$key]
                    TargetTile = $TargetTiles[$key]
                }
            }
        }
    }
    return $transitionTiles
}

# Export functions
Export-ModuleMember -Function @(
    'Initialize-AssetSystem',
    'Expand-GameArchive',
    'Move-GameAsset',
    'Write-Log',
    'Test-GameAsset',
    'Get-AssetStats',
    'Test-LevelTemplate',
    'Get-AssetChunk',
    'Get-PooledResource',
    'Test-TemplateCompatibility',
    'Get-ScaledTemplate',
    'New-TemplateTransition'
) 