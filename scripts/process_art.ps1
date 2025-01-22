# Process and organize art assets for Shadow Worker
# This script extracts and organizes art assets into the correct structure

# Set working directory to script location
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location -Path $scriptPath

# Check for 7-Zip
$sevenZipPath = "C:\Program Files\7-Zip\7z.exe"
if (-not (Test-Path -Path $sevenZipPath)) {
    Write-Warning "7-Zip not found at $sevenZipPath. Installing via winget..."
    try {
        Start-Process "winget" -ArgumentList "install -e --id 7zip.7zip" -Wait -NoNewWindow
    }
    catch {
        Write-Warning "Failed to install 7-Zip. RAR files will be skipped."
    }
}

# Import asset management module
$modulePath = Join-Path -Path $scriptPath -ChildPath "AssetManagement"
Import-Module -Name $modulePath -Force

# Initialize directories
Initialize-AssetSystem -CreateBackup

# Asset categories and their source packs
$assetMapping = @{
    "sprites/environment/tiles" = @(
        "RF_Catacombs_v1.0.zip",  # Main dungeon tileset
        "Pixel Art Top Down - Basic v1.1.2.zip"  # Additional environment tiles
    )
    "sprites/characters/enemies" = @(
        "Monster Creature sprites (pack 1 by batareya).rar",
        "Free-Chaos-Monsters-32x32-Icon-Pack.zip",
        "Skeleton Sprite Pack.rar"
    )
    "sprites/characters/npcs" = @(
        "Characters 500+ (by batareya).rar",
        "free-fairy-avatar-icons-32x32-pixel-art.zip"
    )
    "sprites/characters/player" = @(
        "The Adventurer - Free.zip",
        "Blue Witch.zip"
    )
    "sprites/environment/props" = @(
        "Free-Undead-Loot-Game-Icons.zip",
        "Humble Gift.zip"
    )
}

# Process each category
foreach ($category in $assetMapping.Keys) {
    Write-Host "Processing category: $category"
    $targetDir = Join-Path -Path ".." -ChildPath "art" | Join-Path -ChildPath $category
    
    # Create category directory
    New-Item -ItemType Directory -Force -Path $targetDir | Out-Null
    
    # Process each pack in the category
    foreach ($pack in $assetMapping[$category]) {
        $packPath = Join-Path -Path ".." -ChildPath "art" | Join-Path -ChildPath $pack
        if (Test-Path -Path $packPath) {
            Write-Host "Extracting: $pack"
            
            # Create temp directory for extraction
            $tempDir = Join-Path -Path $env:TEMP -ChildPath "GameAssetExtraction_$([System.IO.Path]::GetFileNameWithoutExtension($pack))"
            New-Item -ItemType Directory -Force -Path $tempDir | Out-Null
            
            # Extract based on extension
            switch ([System.IO.Path]::GetExtension($pack)) {
                ".zip" {
                    Expand-Archive -Path $packPath -DestinationPath $tempDir -Force
                }
                ".rar" {
                    if (Test-Path -Path $sevenZipPath) {
                        & $sevenZipPath x $packPath "-o$tempDir" -y | Out-Null
                    } else {
                        Write-Warning "7-Zip not found. Skipping RAR file: $pack"
                        continue
                    }
                }
            }
            
            # Import assets from temp directory
            Import-GameAssets -SourcePath $tempDir -Category $category -AllowedExtensions @('.png','.jpg')
            
            # Cleanup
            Remove-Item -Path $tempDir -Recurse -Force
        } else {
            Write-Warning "Pack not found: $pack"
        }
    }
}

# Create resources directory if it doesn't exist
$resourcesDir = Join-Path -Path ".." -ChildPath "resources"
if (-not (Test-Path -Path $resourcesDir)) {
    New-Item -ItemType Directory -Force -Path $resourcesDir | Out-Null
}

# Create Godot resource files
$tilesetContent = @"
[gd_resource type="TileSet" load_steps=3 format=3 uid="uid://bc3fsh3mj5nyq"]

[ext_resource type="Texture2D" uid="uid://b8j6v3q4m5n2y" path="res://art/sprites/environment/tiles/tileset_dungeon.png" id="1_tiles"]

[sub_resource type="TileSetAtlasSource" id="TileSetAtlasSource_1"]
resource_name = "Dungeon"
texture = ExtResource("1_tiles")
texture_region_size = Vector2i(32, 32)
0:0/0 = 0
0:0/0/physics_layer_0/linear_velocity = Vector2(0, 0)
0:0/0/physics_layer_0/angular_velocity = 0.0
0:0/0/physics_layer_0/polygon_0/points = PackedVector2Array(-16, -16, 16, -16, 16, 16, -16, 16)
1:0/0 = 0
1:0/0/physics_layer_0/linear_velocity = Vector2(0, 0)
1:0/0/physics_layer_0/angular_velocity = 0.0
2:0/0 = 0
2:0/0/physics_layer_0/linear_velocity = Vector2(0, 0)
2:0/0/physics_layer_0/angular_velocity = 0.0

[resource]
tile_size = Vector2i(32, 32)
physics_layer_0/collision_layer = 1
physics_layer_0/collision_mask = 14
sources/0 = SubResource("TileSetAtlasSource_1")
"@

Set-Content -Path (Join-Path -Path $resourcesDir -ChildPath "tileset.tres") -Value $tilesetContent

# Create scripts directory if it doesn't exist
$scriptsDir = Join-Path -Path ".." -ChildPath "scripts"
if (-not (Test-Path -Path $scriptsDir)) {
    New-Item -ItemType Directory -Force -Path $scriptsDir | Out-Null
}

# Create world generation script with asset references
$worldGenContent = @"
extends Node2D

# Asset paths
const TILE_SETS = {
    "dungeon": preload("res://resources/tileset.tres")
}

const ENEMIES = {
    "skeleton": preload("res://art/sprites/characters/enemies/skeleton.png"),
    "monster": preload("res://art/sprites/characters/enemies/monster.png"),
    "chaos": preload("res://art/sprites/characters/enemies/chaos_monster.png")
}

const NPCS = {
    "villager": preload("res://art/sprites/characters/npcs/villager.png"),
    "fairy": preload("res://art/sprites/characters/npcs/fairy.png")
}

const PROPS = {
    "chest": preload("res://art/sprites/environment/props/chest.png"),
    "torch": preload("res://art/sprites/environment/props/torch.png"),
    "barrel": preload("res://art/sprites/environment/props/barrel.png")
}

func _ready():
    # Initialize world with assets
    var tilemap = $TileMap
    tilemap.tile_set = TILE_SETS["dungeon"]
    
    # Place initial props and NPCs
    _place_props()
    _place_npcs()
    _place_enemies()

func _place_props():
    # Place decorative and interactive props
    for prop_name in PROPS.keys():
        var sprite = Sprite2D.new()
        sprite.texture = PROPS[prop_name]
        add_child(sprite)
        # TODO: Set position based on room layout

func _place_npcs():
    # Place NPCs in appropriate locations
    for npc_name in NPCS.keys():
        var sprite = Sprite2D.new()
        sprite.texture = NPCS[npc_name]
        add_child(sprite)
        # TODO: Set position based on room type

func _place_enemies():
    # Place enemies based on difficulty and room type
    for enemy_name in ENEMIES.keys():
        var sprite = Sprite2D.new()
        sprite.texture = ENEMIES[enemy_name]
        add_child(sprite)
        # TODO: Set position based on spawn points
"@

Set-Content -Path (Join-Path -Path $scriptsDir -ChildPath "world_generation.gd") -Value $worldGenContent

Write-Host "Asset processing complete. Run Godot to import the assets." 