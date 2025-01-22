@tool
extends Control

const ASSET_ROOT = "res://assets"
const ART_ROOT = "res://art"

func _ready():
    var vbox = VBoxContainer.new()
    add_child(vbox)
    
    var label = Label.new()
    label.text = "Asset Manager"
    vbox.add_child(label)
    
    var reorganize_button = Button.new()
    reorganize_button.text = "Reorganize Assets"
    reorganize_button.pressed.connect(_on_reorganize_pressed)
    vbox.add_child(reorganize_button)
    
    var status_label = Label.new()
    status_label.name = "StatusLabel"
    status_label.text = "Ready"
    vbox.add_child(status_label)

func _on_reorganize_pressed():
    var status_label = get_node("StatusLabel")
    status_label.text = "Starting asset reorganization..."
    
    print("Creating directory structure...")
    create_directory_structure()
    
    print("Migrating assets...")
    migrate_assets()
    
    print("Updating resource paths...")
    update_resource_paths()
    
    status_label.text = "Asset reorganization complete!"
    
    # Refresh the editor
    EditorInterface.get_resource_filesystem().scan()

func create_directory_structure():
    var dirs = [
        "sprites/characters/player",
        "sprites/characters/npcs",
        "sprites/environment/tiles",
        "sprites/environment/props",
        "sprites/effects",
        "audio/music",
        "audio/sfx"
    ]
    
    for dir in dirs:
        var art_path = ART_ROOT.path_join(dir)
        var asset_path = ASSET_ROOT.path_join(dir)
        ensure_directory(art_path)
        ensure_directory(asset_path)

func ensure_directory(path: String) -> void:
    if not DirAccess.dir_exists_absolute(path):
        var err = DirAccess.make_dir_recursive_absolute(path)
        if err == OK:
            print("Created directory: ", path)
        else:
            push_error("Failed to create directory: ", path)

func migrate_assets() -> void:
    var migrations = {
        "res://assets/player": {
            "destination": "res://art/sprites/characters/player",
            "validate": func(file_path): return file_path.ends_with(".png") or file_path.ends_with(".import")
        },
        "res://assets/tiles": {
            "destination": "res://art/sprites/environment/tiles",
            "validate": func(file_path): return file_path.ends_with(".png") or file_path.ends_with(".import")
        }
    }
    
    for old_path in migrations:
        if DirAccess.dir_exists_absolute(old_path):
            var migration = migrations[old_path]
            ensure_directory(migration.destination)
            
            var dir = DirAccess.open(old_path)
            if dir:
                dir.list_dir_begin()
                var file_name = dir.get_next()
                
                while file_name != "":
                    if not dir.current_is_dir():
                        var source_path = old_path.path_join(file_name)
                        if migration.validate.call(source_path):
                            var dest_path = migration.destination.path_join(file_name)
                            
                            if FileAccess.file_exists(dest_path):
                                var backup_path = dest_path + ".backup"
                                DirAccess.copy_absolute(dest_path, backup_path)
                            
                            var err = DirAccess.copy_absolute(source_path, dest_path)
                            if err == OK:
                                DirAccess.remove_absolute(source_path)
                    
                    file_name = dir.get_next()
                
                dir.list_dir_end()

func update_resource_paths() -> void:
    var scene_dir = DirAccess.open("res://scenes")
    if scene_dir:
        update_directory_resource_paths(scene_dir)
    
    var script_dir = DirAccess.open("res://scripts")
    if script_dir:
        update_directory_resource_paths(script_dir)

func update_directory_resource_paths(dir: DirAccess) -> void:
    dir.list_dir_begin()
    var file_name = dir.get_next()
    
    while file_name != "":
        if not dir.current_is_dir():
            if file_name.ends_with(".tscn") or file_name.ends_with(".gd"):
                update_file_resource_paths(dir.get_current_dir().path_join(file_name))
        file_name = dir.get_next()
    
    dir.list_dir_end()

func update_file_resource_paths(file_path: String) -> void:
    var content = FileAccess.get_file_as_string(file_path)
    var updated = false
    
    var path_updates = {
        "res://assets/player": "res://art/sprites/characters/player",
        "res://assets/tiles": "res://art/sprites/environment/tiles"
    }
    
    for old_path in path_updates:
        if content.contains(old_path):
            content = content.replace(old_path, path_updates[old_path])
            updated = true
    
    if updated:
        var file = FileAccess.open(file_path, FileAccess.WRITE)
        if file:
            file.store_string(content)
            file.close() 