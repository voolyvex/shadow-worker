extends TileMap

enum TileType {
	FLOOR = 0,
	WALL = 1,
	DOOR = 2,
	WINDOW = 3
}

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

# Set a tile at the given position with the specified type
func set_tile(pos: Vector2i, type: TileType) -> void:
	set_cell(0, pos, 0, Vector2i(type, 0))

# Get the tile type at the given position
func get_tile_type(pos: Vector2i) -> TileType:
	var cell_data = get_cell_tile_data(0, pos)
	if cell_data == null:
		return TileType.FLOOR
	return cell_data.get_custom_data("type")

# Clear all tiles in a rectangular area
func clear_area(start: Vector2i, end: Vector2i) -> void:
	for x in range(start.x, end.x + 1):
		for y in range(start.y, end.y + 1):
			erase_cell(0, Vector2i(x, y))

# Fill a rectangular area with the specified tile type
func fill_area(start: Vector2i, end: Vector2i, type: TileType) -> void:
	for x in range(start.x, end.x + 1):
		for y in range(start.y, end.y + 1):
			set_tile(Vector2i(x, y), type)

# Create a room with walls and floor
func create_room(pos: Vector2i, size: Vector2i) -> void:
	# Fill the entire area with floor tiles
	fill_area(pos, pos + size, TileType.FLOOR)
	
	# Add walls around the perimeter
	for x in range(pos.x, pos.x + size.x + 1):
		set_tile(Vector2i(x, pos.y), TileType.WALL)  # Top wall
		set_tile(Vector2i(x, pos.y + size.y), TileType.WALL)  # Bottom wall
	
	for y in range(pos.y, pos.y + size.y + 1):
		set_tile(Vector2i(pos.x, y), TileType.WALL)  # Left wall
		set_tile(Vector2i(pos.x + size.x, y), TileType.WALL)  # Right wall

# Add a door to a wall
func add_door(pos: Vector2i) -> void:
	if get_tile_type(pos) == TileType.WALL:
		set_tile(pos, TileType.DOOR)

# Add a window to a wall
func add_window(pos: Vector2i) -> void:
	if get_tile_type(pos) == TileType.WALL:
		set_tile(pos, TileType.WINDOW)

# Check if a position is walkable (floor or door)
func is_walkable(pos: Vector2i) -> bool:
	var type = get_tile_type(pos)
	return type == TileType.FLOOR or type == TileType.DOOR 
