extends Node2D

@onready var world_system = $WorldSystem
@onready var player = $Player
@onready var stamina_bar = $UI/StaminaBar

func _ready():
	# Set up world generation parameters
	world_system.room_size = Vector2i(6, 6)
	world_system.room_count = 5
	world_system.object_density = 0.15
	world_system.npc_scene = "res://scenes/npc.tscn"
	world_system.min_npcs_per_room = 1
	world_system.max_npcs_per_room = 3
	
	# Register placeable objects
	world_system.add_placeable_object({
		"scene_path": "res://scenes/objects/container.tscn",
		"type": Constants.ObjectType.CONTAINER,
		"spawn_weight": 1.0,
		"requires_wall": true,
		"blocks_path": true
	})
	
	world_system.add_placeable_object({
		"scene_path": "res://scenes/objects/workbench.tscn",
		"type": Constants.ObjectType.WORKSTATION,
		"spawn_weight": 0.7,
		"requires_wall": true,
		"blocks_path": true
	})
	
	world_system.add_placeable_object({
		"scene_path": "res://scenes/objects/crate.tscn",
		"type": Constants.ObjectType.OBSTACLE,
		"spawn_weight": 0.8,
		"requires_wall": false,
		"blocks_path": true
	})
	
	world_system.add_placeable_object({
		"scene_path": "res://scenes/objects/lamp.tscn",
		"type": Constants.ObjectType.LIGHT_SOURCE,
		"spawn_weight": 0.5,
		"requires_wall": true,
		"blocks_path": false
	})
	
	world_system.add_placeable_object({
		"scene_path": "res://scenes/objects/plant.tscn",
		"type": Constants.ObjectType.DECORATION,
		"spawn_weight": 0.3,
		"requires_wall": false,
		"blocks_path": false
	})
	
	# Generate the world with a random seed
	var seed = randi()
	world_system.generate_world(seed)
	
	# Connect player signals
	player.stamina_changed.connect(_on_player_stamina_changed)
	stamina_bar.value = player.get_stamina() * 100

func _on_player_stamina_changed(value):
	stamina_bar.value = value * 100 
