extends Node2D

@onready var tilemap: TileMap = $TileMap

const LAYER_GROUND := 0
const LAYER_WALLS := 1

func _ready() -> void:
	print("Starting tilemap test...")
	generate_test_room()
	
func generate_test_room() -> void:
	var room_size := Vector2i(7, 7)
	var room_pos := Vector2i(0, 0)
	
	# Place floor tiles
	for x in range(room_size.x):
		for y in range(room_size.y):
			var pos := room_pos + Vector2i(x, y)
			tilemap.set_cell(LAYER_GROUND, pos, 0, Vector2i(0, 0))
	
	# Place wall tiles
	_place_walls(room_pos, room_size)
	
	print("Created room with size: ", room_size)
	print("Floor tiles placed: ", tilemap.get_used_cells(LAYER_GROUND).size())
	print("Wall tiles placed: ", tilemap.get_used_cells(LAYER_WALLS).size())
	print("Test completed")

func _place_walls(pos: Vector2i, size: Vector2i) -> void:
	# Top and bottom walls - horizontal
	for x in range(size.x):
		tilemap.set_cell(LAYER_WALLS, pos + Vector2i(x, -1), 0, Vector2i(2, 0))
		tilemap.set_cell(LAYER_WALLS, pos + Vector2i(x, size.y), 0, Vector2i(2, 0))
	
	# Left and right walls - vertical
	for y in range(size.y):
		tilemap.set_cell(LAYER_WALLS, pos + Vector2i(-1, y), 0, Vector2i(1, 0))
		tilemap.set_cell(LAYER_WALLS, pos + Vector2i(size.x, y), 0, Vector2i(1, 0)) 
