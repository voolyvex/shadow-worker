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
	if tilemap:
		tilemap.tile_set = TILE_SETS["dungeon"]
		print("Initialized tilemap with dungeon tileset")
	else:
		push_error("TileMap node not found")
		return
	
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
