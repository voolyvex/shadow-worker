extends Node2D

@export var room_size: Vector2i = Vector2i(6, 6)
@export var room_count: int = 5
@export var object_density: float = 0.15
@export var npc_scene: String = ""
@export var min_npcs_per_room: int = 1
@export var max_npcs_per_room: int = 3

var placeable_objects = []
var rooms = []
var current_room = null

@onready var tilemap = $TileMap

func _ready():
    randomize()

func add_placeable_object(object_data: Dictionary):
    placeable_objects.append(object_data)

func generate_world(seed_value: int = 0):
    if seed_value != 0:
        seed(seed_value)
    
    # Clear existing world
    rooms.clear()
    tilemap.clear()
    
    # Generate rooms
    for i in range(room_count):
        var room = {
            "position": Vector2i(i * (room_size.x + 2), 0),
            "size": room_size,
            "connections": [],
            "occupied_positions": []
        }
        rooms.append(room)
        generate_room(room)
    
    # Connect rooms
    for i in range(rooms.size() - 1):
        connect_rooms(rooms[i], rooms[i + 1])
    
    # Place objects and NPCs
    for room in rooms:
        place_objects(room)
        place_npcs(room)

func generate_room(room: Dictionary):
    var pos = room.position
    var size = room.size
    
    # Generate walls and floor
    for x in range(-1, size.x + 1):
        for y in range(-1, size.y + 1):
            var tile_pos = Vector2i(pos.x + x, pos.y + y)
            if x == -1 or x == size.x or y == -1 or y == size.y:
                # Place wall tile
                tilemap.set_cell(1, tile_pos, 0, Vector2i(1, 0))
            else:
                # Place floor tile
                tilemap.set_cell(0, tile_pos, 0, Vector2i(0, 0))

func connect_rooms(room1: Dictionary, room2: Dictionary):
    var pos1 = room1.position
    var pos2 = room2.position
    var size1 = room1.size
    
    # Create a corridor
    var corridor_start = Vector2i(pos1.x + size1.x, pos1.y + size1.y / 2)
    var corridor_end = Vector2i(pos2.x, pos2.y + room2.size.y / 2)
    
    for x in range(corridor_start.x, corridor_end.x + 1):
        # Place floor tile
        tilemap.set_cell(0, Vector2i(x, corridor_start.y), 0, Vector2i(0, 0))
        # Place wall tiles
        tilemap.set_cell(1, Vector2i(x, corridor_start.y - 1), 0, Vector2i(1, 0))
        tilemap.set_cell(1, Vector2i(x, corridor_start.y + 1), 0, Vector2i(1, 0))
    
    room1.connections.append(room2)
    room2.connections.append(room1)

func place_objects(room: Dictionary):
    var available_positions = []
    var pos = room.position
    var size = room.size
    
    # Get available floor positions
    for x in range(size.x):
        for y in range(size.y):
            var tile_pos = Vector2i(pos.x + x, pos.y + y)
            if tilemap.get_cell_source_id(0, tile_pos) == 0:
                available_positions.append(tile_pos)
    
    # Calculate number of objects to place
    var num_objects = int(available_positions.size() * object_density)
    
    # Place objects
    for i in range(num_objects):
        if available_positions.is_empty():
            break
            
        # Select random object based on weights
        var total_weight = 0.0
        for obj in placeable_objects:
            total_weight += obj.spawn_weight
        
        var random_weight = randf() * total_weight
        var selected_object = null
        
        for obj in placeable_objects:
            random_weight -= obj.spawn_weight
            if random_weight <= 0:
                selected_object = obj
                break
        
        if selected_object == null:
            continue
        
        # Find valid position
        var valid_positions = available_positions.duplicate()
        while not valid_positions.is_empty():
            var index = randi() % valid_positions.size()
            var pos_to_try = valid_positions[index]
            
            if is_valid_object_position(pos_to_try, selected_object, room):
                # Load and place object
                var scene = load(selected_object.scene_path)
                if scene:
                    var object = scene.instantiate()
                    add_child(object)
                    object.position = tilemap.map_to_local(pos_to_try)
                    
                    # Update occupied positions
                    room.occupied_positions.append(pos_to_try)
                    available_positions.erase(pos_to_try)
                break
            
            valid_positions.remove_at(index)

func place_npcs(room: Dictionary):
    var available_positions = []
    var pos = room.position
    var size = room.size
    
    # Get available floor positions
    for x in range(size.x):
        for y in range(size.y):
            var tile_pos = Vector2i(pos.x + x, pos.y + y)
            if tilemap.get_cell_source_id(0, tile_pos) == 0 and not tile_pos in room.occupied_positions:
                available_positions.append(tile_pos)
    
    # Place NPCs
    var num_npcs = randi_range(min_npcs_per_room, max_npcs_per_room)
    for i in range(num_npcs):
        if available_positions.is_empty():
            break
        
        var index = randi() % available_positions.size()
        var npc_pos = available_positions[index]
        
        # Load and place NPC
        var scene = load(npc_scene)
        if scene:
            var npc = scene.instantiate()
            add_child(npc)
            npc.position = tilemap.map_to_local(npc_pos)
            
            # Update occupied positions
            room.occupied_positions.append(npc_pos)
            available_positions.remove_at(index)

func is_valid_object_position(pos: Vector2i, object: Dictionary, room: Dictionary) -> bool:
    # Check if position is already occupied
    if pos in room.occupied_positions:
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