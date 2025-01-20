# Asset Management Module
using namespace System.IO

# Module Variables
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

# Helper Functions
function Write-Log {
    param(
        [string]$Message,
        [string]$Level = "Info"
    )
    
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logDir = Join-Path $PSScriptRoot "..\logs\$(Get-Date -Format 'yyyy-MM-dd')"
    
    if (-not (Test-Path $logDir)) {
        New-Item -ItemType Directory -Force -Path $logDir | Out-Null
    }
    
    $logPath = Join-Path $logDir "errors.log"
    $formattedMessage = "[$timestamp] [$Level] $Message"
    Add-Content -Path $logPath -Value $formattedMessage
}

function Get-BlendedTiles {
    param(
        [PSObject]$SourceTiles,
        [PSObject]$TargetTiles,
        [float]$BlendFactor = 0.5
    )
    
    $blendedTiles = @{
        walls = @()
        obstacles = @()
    }
    
    # Blend walls
    foreach ($wall in $SourceTiles.walls) {
        $targetWall = $TargetTiles.walls | Where-Object { $_.x -eq $wall.x -and $_.y -eq $wall.y }
        if ($targetWall) {
            $blendedTiles.walls += @{
                x = $wall.x
                y = $wall.y
                type = $wall.type
            }
        }
    }
    
    # Blend obstacles
    foreach ($obstacle in $SourceTiles.obstacles) {
        $targetObstacle = $TargetTiles.obstacles | Where-Object { 
            $_.position.x -eq $obstacle.position.x -and $_.position.y -eq $obstacle.position.y 
        }
        if ($targetObstacle) {
            $blendedProperties = @{}
            foreach ($prop in $obstacle.properties.PSObject.Properties) {
                if ($targetObstacle.properties.PSObject.Properties[$prop.Name]) {
                    $sourceVal = $obstacle.properties.($prop.Name)
                    $targetVal = $targetObstacle.properties.($prop.Name)
                    if ($sourceVal -is [int] -or $sourceVal -is [float]) {
                        $blendedProperties[$prop.Name] = $sourceVal * (1 - $BlendFactor) + $targetVal * $BlendFactor
                    } else {
                        $blendedProperties[$prop.Name] = if ($BlendFactor -ge 0.5) { $targetVal } else { $sourceVal }
                    }
                }
            }
            
            $blendedTiles.obstacles += @{
                type = $obstacle.type
                position = $obstacle.position
                properties = $blendedProperties
            }
        }
    }
    
    return $blendedTiles
}

function Get-DirectTransitionTiles {
    param(
        [PSObject]$SourceTiles,
        [PSObject]$TargetTiles
    )
    
    $transitionTiles = @{
        walls = @()
        obstacles = @()
    }
    
    # Process walls
    foreach ($wall in $SourceTiles.walls) {
        $targetWall = $TargetTiles.walls | Where-Object { $_.x -eq $wall.x -and $_.y -eq $wall.y }
        if ($targetWall) {
            $transitionTiles.walls += @{
                x = $wall.x
                y = $wall.y
                type = "transition"
                source = $wall
                target = $targetWall
            }
        }
    }
    
    # Process obstacles
    foreach ($obstacle in $SourceTiles.obstacles) {
        $targetObstacle = $TargetTiles.obstacles | Where-Object { 
            $_.position.x -eq $obstacle.position.x -and $_.position.y -eq $obstacle.position.y 
        }
        if ($targetObstacle) {
            $transitionTiles.obstacles += @{
                type = "transition"
                position = $obstacle.position
                properties = @{
                    source = $obstacle
                    target = $targetObstacle
                }
            }
        }
    }
    
    return $transitionTiles
}

function Test-TemplateCompatibility {
    param(
        [string]$TemplatePath,
        [string]$TargetSize
    )
    
    try {
        $template = Get-Content $TemplatePath | ConvertFrom-Json
        
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
        $template = Get-Content $TemplatePath | ConvertFrom-Json
        $targetDimensions = $script:TemplateConfig.Sizes[$TargetSize]
        
        # Create a new template object
        $scaledTemplate = @{
            type = $template.type
            difficulty = $template.difficulty
            size = @{
                x = $targetDimensions[0]
                y = $targetDimensions[1]
            }
            connections = $template.connections
            spawns = $template.spawns
            tiles = $template.tiles
            lighting = $template.lighting
            metadata = $template.metadata
            properties = @{
                scaling = $script:TemplateConfig.ScaleModes[$ScaleMode]
            }
        }
        
        return $scaledTemplate
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
        $source = Get-Content $SourcePath | ConvertFrom-Json
        $target = Get-Content $TargetPath | ConvertFrom-Json
        
        $transitionTiles = switch ($TransitionType) {
            "Smooth" { Get-BlendedTiles -SourceTiles $source.tiles -TargetTiles $target.tiles }
            "Sharp" { Get-DirectTransitionTiles -SourceTiles $source.tiles -TargetTiles $target.tiles }
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

# Export module members
Export-ModuleMember -Function @(
    'Test-TemplateCompatibility',
    'Get-ScaledTemplate',
    'New-TemplateTransition',
    'Write-Log'
) 