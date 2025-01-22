@tool
extends EditorScript

const ASSET_ROOT = "res://assets"
const ART_ROOT = "res://art"

func _run():
	print("Starting asset reorganization...")
	var editor_interface = get_editor_interface()
	if not editor_interface:
		push_error("Could not access editor interface!")
		return
		
	print("Creating directory structure...")
	create_directory_structure()
	print("Migrating assets...")
	migrate_assets()
	print("Updating resource paths...")
	update_resource_paths()
	print("Asset reorganization complete!")
	
	# Refresh the editor to show changes
	if editor_interface.get_resource_filesystem():
		editor_interface.get_resource_filesystem().scan()

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
		print("Created directories: ", art_path, " and ", asset_path)

func ensure_directory(path: String) -> void:
	if not DirAccess.dir_exists_absolute(path):
		var err = DirAccess.make_dir_recursive_absolute(path)
		if err == OK:
			print("Created directory: ", path)
		else:
			push_error("Failed to create directory: ", path, " Error: ", err)

func migrate_assets() -> void:
	print("Starting asset migration...")
	
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
			print("Migrating assets from ", old_path, " to ", migration.destination)
			
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
							print("Moving ", source_path, " to ", dest_path)
							
							if FileAccess.file_exists(dest_path):
								print("Warning: File already exists at destination: ", dest_path)
								var backup_path = dest_path + ".backup"
								var err = DirAccess.copy_absolute(dest_path, backup_path)
								if err != OK:
									push_error("Failed to create backup: ", backup_path)
									file_name = dir.get_next()
									continue
							
							var err = DirAccess.copy_absolute(source_path, dest_path)
							if err == OK:
								print("Successfully moved ", file_name)
								DirAccess.remove_absolute(source_path)
							else:
								push_error("Failed to move ", file_name, " Error: ", err)
					
					file_name = dir.get_next()
				
				dir.list_dir_end()
			else:
				push_error("Could not access directory: ", old_path)

func update_resource_paths() -> void:
	print("Updating resource paths...")
	
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
		print("Updating resource paths in: ", file_path)
		var file = FileAccess.open(file_path, FileAccess.WRITE)
		if file:
			file.store_string(content)
			file.close()
		else:
			push_error("Failed to write to file: ", file_path)

func _generate_player_sprite():
	# Generate player sprite
	var player_image = Image.create(32, 32, false, Image.FORMAT_RGBA8)
	player_image.fill(Color(0.2, 0.6, 1.0, 1.0))  # Blue player
	# Add a simple shape
	for x in range(8, 24):
		for y in range(8, 24):
			player_image.set_pixel(x, y, Color(0.3, 0.7, 1.0, 1.0))
	player_image.save_png("res://assets/player/player.png")
	
func _generate_tileset():
	# Generate tileset
	var tileset_image = Image.create(64, 32, false, Image.FORMAT_RGBA8)
	# Floor tile (gray)
	for x in range(32):
		for y in range(32):
			var color = Color(0.8, 0.8, 0.8, 1.0)
			if (x + y) % 4 == 0:
				color = Color(0.7, 0.7, 0.7, 1.0)
			tileset_image.set_pixel(x, y, color)
	# Wall tile (brown)
	for x in range(32, 64):
		for y in range(32):
			var color = Color(0.6, 0.4, 0.2, 1.0)
			if (x + y) % 4 == 0:
				color = Color(0.5, 0.3, 0.1, 1.0)
			tileset_image.set_pixel(x, y, color)
	tileset_image.save_png("res://assets/tiles/tileset.png")
	
	print("Assets generated successfully!") 
