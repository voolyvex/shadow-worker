# Create archive directory
New-Item -ItemType Directory -Force -Path "_godot_archive"

# Move Godot-specific files and directories
Move-Item -Path "scripts" -Destination "_godot_archive" -Force
Move-Item -Path "scenes" -Destination "_godot_archive" -Force
Move-Item -Path ".godot" -Destination "_godot_archive" -Force
Move-Item -Path "project.godot" -Destination "_godot_archive" -Force
Move-Item -Path "icon.svg" -Destination "_godot_archive" -Force
Move-Item -Path "SConstruct" -Destination "_godot_archive" -Force
Move-Item -Path "*.gdextension" -Destination "_godot_archive" -Force

# Move docs except unity architecture
New-Item -ItemType Directory -Force -Path "_godot_archive/docs"
Move-Item -Path "docs/architecture.md" -Destination "_godot_archive/docs" -Force
Move-Item -Path "docs/unity_transition.md" -Destination "_godot_archive/docs" -Force

# Create Unity project structure
$directories = @(
    "Assets/Scripts/Core/PersonalitySystem",
    "Assets/Scripts/Core/WorldSystem",
    "Assets/Scripts/Core/Managers",
    "Assets/Scripts/Player",
    "Assets/Scripts/UI",
    "Assets/Scripts/Effects",
    "Assets/Scripts/Utils",
    "Assets/Scripts/Tests/EditMode",
    "Assets/Scripts/Tests/PlayMode",
    "Assets/Resources/Tiles",
    "Assets/Resources/Effects",
    "Assets/Prefabs/Player",
    "Assets/Prefabs/NPC",
    "Assets/Prefabs/Objects"
)

foreach ($dir in $directories) {
    New-Item -ItemType Directory -Force -Path $dir
    New-Item -ItemType File -Force -Path "$dir/.gitkeep"
}

Write-Host "Godot files archived and Unity structure created" 