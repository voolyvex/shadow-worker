extends Node2D

@export var room_size: Vector2i = Vector2i(6, 6)
@export var room_count: int = 5
@export var object_density: float = 0.15
@export var npc_scene: String = "res://scenes/npc.tscn"
@export var min_npcs_per_room: int = 1
@export var max_npcs_per_room: int = 3
@export var room_spacing: int = 2  # Space between rooms
@export var chunk_size: Vector2i = Vector2i(32, 32)  # Size of each chunk for loading

var placeable_objects = []
var rooms = []
var current_room = null
var npc_personalities = ["HELPER", "MERCHANT", "SAGE", "TRICKSTER"]
var tile_size: Vector2
var active_chunks = {}  # Dictionary to track loaded chunks
var chunk_cache = {}   # Cache for generated chunks
var chunk_load_margin = 1  # Number of chunks to load beyond visible area
var chunk_load_queue = []  # Queue for chunks that need to be loaded
var is_loading_chunk = false  # Flag to prevent concurrent chunk loading

@onready var tilemap = $TileMap

func _ready():
	randomize()
	print("World system initialized")
	
	# Verify tilemap setup
	if !tilemap:
		push_error("TileMap node not found!")
		return
	
	tile_size = tilemap.tile_set.tile_size
	print("TileMap found and initialized")
	print("Room size: ", room_size)
	print("Room count: ", room_count)
	print("Object density: ", object_density)
	
	# Center the world
	position = Vector2.ZERO
	
	# Configure tilemap for efficient rendering
	tilemap.set_layer_enabled(0, true)  # Ground layer
	tilemap.set_layer_enabled(1, true)  # Wall layer
	
func _process(_delta):
	if not tilemap:
		return
		
	# Lock tilemap position to prevent drifting
	tilemap.position = Vector2.ZERO
	
	# Process chunk loading queue
	if not is_loading_chunk and not chunk_load_queue.is_empty():
		var next_chunk = chunk_load_queue.pop_front()
		_load_chunk_immediate(next_chunk)
	
	# Update visible chunks relative to camera
	var camera = get_viewport().get_camera_2d()
	if camera:
		var viewport_rect = get_viewport_rect()
		var top_left = camera.get_screen_center_position() - viewport_rect.size / 2
		var bottom_right = camera.get_screen_center_position() + viewport_rect.size / 2
		
		# Convert screen coordinates to chunk coordinates with margin
		var start_chunk = world_to_chunk(top_left - Vector2(chunk_load_margin * chunk_size.x * tile_size.x, 
														   chunk_load_margin * chunk_size.y * tile_size.y))
		var end_chunk = world_to_chunk(bottom_right + Vector2(chunk_load_margin * chunk_size.x * tile_size.x,
															  chunk_load_margin * chunk_size.y * tile_size.y))
		
		update_visible_chunks(start_chunk, end_chunk)

func update_visible_chunks(start_chunk: Vector2i, end_chunk: Vector2i):
	var chunks_to_load = {}
	
	# Calculate which chunks should be visible
	for x in range(start_chunk.x, end_chunk.x + 1):
		for y in range(start_chunk.y, end_chunk.y + 1):
			var chunk_pos = Vector2i(x, y)
			chunks_to_load[chunk_pos] = true
	
	# Unload chunks that are no longer visible
	for chunk_pos in active_chunks.keys():
		if not chunks_to_load.has(chunk_pos):
			unload_chunk(chunk_pos)
	
	# Load new chunks that have become visible
	for chunk_pos in chunks_to_load:
		if not active_chunks.has(chunk_pos):
			load_chunk(chunk_pos)

func world_to_chunk(world_pos: Vector2) -> Vector2i:
	return Vector2i(
		floor(world_pos.x / (chunk_size.x * tile_size.x)),
		floor(world_pos.y / (chunk_size.y * tile_size.y))
	)

func load_chunk(chunk_pos: Vector2i):
	if active_chunks.has(chunk_pos) or chunk_pos in chunk_load_queue:
		return
		
	chunk_load_queue.push_back(chunk_pos)

func _load_chunk_immediate(chunk_pos: Vector2i):
	is_loading_chunk = true
	
	if chunk_cache.has(chunk_pos):
		# Restore from cache
		var chunk_data = chunk_cache[chunk_pos]
		for layer in chunk_data.keys():
			for pos in chunk_data[layer].keys():
				var cell_data = chunk_data[layer][pos]
				tilemap.set_cell(layer, pos, cell_data.source_id, cell_data.atlas_coords, 0)
	else:
		# Generate new chunk
		generate_chunk(chunk_pos)
	
	active_chunks[chunk_pos] = true
	is_loading_chunk = false

func unload_chunk(chunk_pos: Vector2i):
	if !active_chunks.has(chunk_pos):
		return
		
	# Remove from load queue if present
	var queue_index = chunk_load_queue.find(chunk_pos)
	if queue_index != -1:
		chunk_load_queue.remove_at(queue_index)
	
	# Cache chunk data before unloading
	var chunk_data = {}
	var chunk_rect = get_chunk_rect(chunk_pos)
	
	for layer in range(2):  # Ground and Wall layers
		chunk_data[layer] = {}
		for x in range(chunk_rect.position.x, chunk_rect.end.x):
			for y in range(chunk_rect.position.y, chunk_rect.end.y):
				var pos = Vector2i(x, y)
				var cell = tilemap.get_cell_source_id(layer, pos)
				if cell != -1:
					chunk_data[layer][pos] = {
						"source_id": cell,
						"atlas_coords": tilemap.get_cell_atlas_coords(layer, pos)
					}
					tilemap.erase_cell(layer, pos)
	
	chunk_cache[chunk_pos] = chunk_data
	active_chunks.erase(chunk_pos)

func get_chunk_rect(chunk_pos: Vector2i) -> Rect2i:
	var start = Vector2i(
		chunk_pos.x * chunk_size.x,
		chunk_pos.y * chunk_size.y
	)
	return Rect2i(start, chunk_size)

func generate_chunk(chunk_pos: Vector2i):
	var chunk_rect = get_chunk_rect(chunk_pos)
	var chunk_data = {}
	
	for layer in range(2):
		chunk_data[layer] = {}
		
	# Generate terrain for the chunk
	for x in range(chunk_rect.position.x, chunk_rect.end.x):
		for y in range(chunk_rect.position.y, chunk_rect.end.y):
			var pos = Vector2i(x, y)
			
			# Use noise or other generation method to determine tile type
			var noise_val = randf()  # Replace with proper noise generation
			
			if noise_val < 0.1:  # Wall probability
				tilemap.set_cell(1, pos, 1, Vector2i(0, 0))
				chunk_data[1][pos] = {"source_id": 1, "atlas_coords": Vector2i(0, 0)}
			else:
				tilemap.set_cell(0, pos, 0, Vector2i(0, 0))
				chunk_data[0][pos] = {"source_id": 0, "atlas_coords": Vector2i(0, 0)}
	
	chunk_cache[chunk_pos] = chunk_data
	active_chunks[chunk_pos] = true

func clear_world() -> void:
	print("Clearing world...")
	# Clear all existing entities
	for child in get_children():
		if child != tilemap:  # Keep the tilemap node
			child.queue_free()
	
	# Clear tilemap
	if tilemap:
		tilemap.clear()
		print("Tilemap cleared")
	else:
		push_error("Tilemap node not found!")
	
	# Reset internal state
	rooms.clear()
	current_room = null
	placeable_objects.clear()
	print("World cleared")

func get_first_room_center() -> Vector2:
	if rooms.size() > 0:
		var room = rooms[0]
		# Calculate center in world coordinates
		var center_x = (room["position"].x + room_size.x / 2.0) * tile_size.x
		var center_y = (room["position"].y + room_size.y / 2.0) * tile_size.y
		print("First room center: ", Vector2(center_x, center_y))
		return Vector2(center_x, center_y)
	push_error("No rooms available for spawn position!")
	return Vector2.ZERO

func generate_world(params: Dictionary = {}) -> void:
	print("\nGenerating world...")
	
	# Use provided seed or generate new one
	var world_seed = params.get("seed", randi())
	seed(world_seed)
	print("Using seed: ", world_seed)
	
	# Clear any existing world
	clear_world()
	
	# Calculate grid size based on room count
	var grid_size: int = int(ceil(sqrt(float(room_count))))
	print("Grid size: ", grid_size, "x", grid_size)
	
	# Calculate total world size
	var total_width = grid_size * (room_size.x + room_spacing) - room_spacing
	var total_height = grid_size * (room_size.y + room_spacing) - room_spacing
	
	# Calculate offset to center the world
	var offset_x = -total_width / 2
	var offset_y = -total_height / 2
	
	# Generate rooms in a grid pattern
	for i in range(room_count):
		var grid_x = int(i % grid_size)
		var grid_y = int(i / grid_size)
		var room = {
			"position": Vector2i(
				offset_x + grid_x * (room_size.x + room_spacing),
				offset_y + grid_y * (room_size.y + room_spacing)
			),
			"size": room_size,
			"connections": [],
			"occupied_positions": []
		}
		rooms.append(room)
		print("Generating room ", i, " at position ", room["position"])
		generate_room(room)
	
	# Connect rooms in a grid pattern
	for i in range(rooms.size()):
		var current_x = int(i % grid_size)
		var current_y = int(i / grid_size)
		
		# Connect horizontally if not last in row
		if current_x < grid_size - 1 and i + 1 < rooms.size():
			connect_rooms(rooms[i], rooms[i + 1])
		
		# Connect vertically if not last in column
		if current_y < grid_size - 1 and i + grid_size < rooms.size():
			connect_rooms(rooms[i], rooms[i + int(grid_size)])
	
	# Place objects and NPCs
	for i in range(rooms.size()):
		var room = rooms[i]
		print("\nPlacing objects in room ", i)
		place_objects(room)
		print("Placing NPCs in room ", i)
		place_npcs(room)
	
	print("World generation complete")

func generate_room(room: Dictionary) -> void:
	var pos = room["position"]
	var size = room["size"]
	
	print("Generating room at position: ", pos, " with size: ", size)
	
	# Generate walls and floor
	for x in range(-1, size.x + 1):
		for y in range(-1, size.y + 1):
			var tile_pos = Vector2i(pos.x + x, pos.y + y)
			
			if x == -1 or x == size.x or y == -1 or y == size.y:
				# Place wall tile from the wall tileset (source 1)
				tilemap.set_cell(1, tile_pos, 1, Vector2i(0, 0))
			else:
				# Place floor tile from the ground tileset (source 0)
				tilemap.set_cell(0, tile_pos, 0, Vector2i(0, 0))
	
	print("Room generation complete at ", pos)

func connect_rooms(room1: Dictionary, room2: Dictionary) -> void:
	var pos1 = room1["position"]
	var pos2 = room2["position"]
	var size1 = room1["size"]
	
	# Determine if connection is horizontal or vertical
	var is_horizontal = abs(pos2.x - pos1.x) > abs(pos2.y - pos1.y)
	
	if is_horizontal:
		# Horizontal corridor
		var corridor_y = pos1.y + size1.y / 2
		var start_x = pos1.x + size1.x
		var end_x = pos2.x
		
		print("Creating horizontal corridor from x=", start_x, " to x=", end_x, " at y=", corridor_y)
		
		for x in range(start_x, end_x + 1):
			# Place floor tile from ground tileset
			tilemap.set_cell(0, Vector2i(x, corridor_y), 0, Vector2i(0, 0))
			# Place wall tiles from wall tileset
			tilemap.set_cell(1, Vector2i(x, corridor_y - 1), 1, Vector2i(0, 0))
			tilemap.set_cell(1, Vector2i(x, corridor_y + 1), 1, Vector2i(0, 0))
	else:
		# Vertical corridor
		var corridor_x = pos1.x + size1.x / 2
		var start_y = pos1.y + size1.y
		var end_y = pos2.y
		
		print("Creating vertical corridor from y=", start_y, " to y=", end_y, " at x=", corridor_x)
		
		for y in range(start_y, end_y + 1):
			# Place floor tile from ground tileset
			tilemap.set_cell(0, Vector2i(corridor_x, y), 0, Vector2i(0, 0))
			# Place wall tiles from wall tileset
			tilemap.set_cell(1, Vector2i(corridor_x - 1, y), 1, Vector2i(0, 0))
			tilemap.set_cell(1, Vector2i(corridor_x + 1, y), 1, Vector2i(0, 0))
	
	room1["connections"].append(room2)
	room2["connections"].append(room1)
	print("Connected rooms at ", pos1, " and ", pos2)

func place_objects(room: Dictionary) -> void:
	var num_objects = int(room_size.x * room_size.y * object_density)
	print("Planning to place ", num_objects, " objects in room at ", room["position"])
	
	for i in range(num_objects):
		var x = room["position"].x + randi() % room_size.x
		var y = room["position"].y + randi() % room_size.y
		var pos = Vector2i(x, y)
		
		if not pos in room["occupied_positions"]:
			# Place a debug marker for now (we'll add real objects later)
			tilemap.set_cell(1, pos, 0, Vector2i(2, 0))
			room["occupied_positions"].append(pos)
			print("Placed object at ", pos)

func place_npcs(room: Dictionary) -> void:
	var num_npcs = randi() % (max_npcs_per_room - min_npcs_per_room + 1) + min_npcs_per_room
	print("Planning to place ", num_npcs, " NPCs in room at ", room["position"])
	
	for i in range(num_npcs):
		var x = room["position"].x + randi() % room_size.x
		var y = room["position"].y + randi() % room_size.y
		var pos = Vector2i(x, y)
		
		if not pos in room["occupied_positions"]:
			# Instance NPC scene
			var npc_instance = load(npc_scene).instantiate()
			add_child(npc_instance)
			npc_instance.position = Vector2(pos.x * 64 + 32, pos.y * 64 + 32)
			
			# Set random personality
			var personality = npc_personalities[randi() % npc_personalities.size()]
			if npc_instance.has_method("set_personality"):
				npc_instance.set_personality(personality)
			
			room["occupied_positions"].append(pos)
			print("Placed NPC at ", pos, " with personality ", personality)

func add_placeable_object(object_data: Dictionary):
	placeable_objects.append(object_data)
	print("Added placeable object: ", object_data)

func is_valid_object_position(pos: Vector2i, object: Dictionary, room: Dictionary) -> bool:
	# Check if position is already occupied
	if pos in room["occupied_positions"]:
		return false
	
	# Check wall requirement
	if object.requires_wall:
		var has_adjacent_wall = false
		for dir in [Vector2i.UP, Vector2i.RIGHT, Vector2i.DOWN, Vector2i.LEFT]:
			var check_pos = pos + dir
			if tilemap.get_cell_source_id(1, check_pos) == 0:
				has_adjacent_wall = true
				break
		if not has_adjacent_wall:
			return false
	
	return true 
