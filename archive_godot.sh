#!/bin/bash

# Create archive directory
mkdir -p _godot_archive

# Move Godot-specific files and directories
mv scripts/ _godot_archive/
mv scenes/ _godot_archive/
mv .godot/ _godot_archive/
mv project.godot _godot_archive/
mv icon.svg _godot_archive/
mv SConstruct _godot_archive/
mv *.gdextension _godot_archive/

# Move docs except unity architecture
mkdir -p _godot_archive/docs
mv docs/architecture.md _godot_archive/docs/
mv docs/unity_transition.md _godot_archive/docs/

# Create Unity project structure
mkdir -p Assets/{Scripts,Scenes,Prefabs,Resources,Tests}
mkdir -p Assets/Scripts/{Core,Player,UI,Effects,Utils}
mkdir -p Assets/Scripts/Core/{PersonalitySystem,WorldSystem,Managers}
mkdir -p Assets/Scripts/Tests/{EditMode,PlayMode}
mkdir -p Assets/Resources/{Tiles,Effects}
mkdir -p Assets/Prefabs/{Player,NPC,Objects}

# Create placeholder files to preserve directory structure
touch Assets/Scripts/Core/PersonalitySystem/.gitkeep
touch Assets/Scripts/Core/WorldSystem/.gitkeep
touch Assets/Scripts/Core/Managers/.gitkeep
touch Assets/Scripts/Player/.gitkeep
touch Assets/Scripts/UI/.gitkeep
touch Assets/Scripts/Effects/.gitkeep
touch Assets/Scripts/Utils/.gitkeep
touch Assets/Scripts/Tests/EditMode/.gitkeep
touch Assets/Scripts/Tests/PlayMode/.gitkeep
touch Assets/Resources/Tiles/.gitkeep
touch Assets/Resources/Effects/.gitkeep
touch Assets/Prefabs/Player/.gitkeep
touch Assets/Prefabs/NPC/.gitkeep
touch Assets/Prefabs/Objects/.gitkeep

echo "Godot files archived and Unity structure created" 