extends Node2D

@onready var world_system = $WorldSystem
@onready var player = $Player
@onready var stamina_bar = $UI/StaminaBar

func _ready():
	print("Main scene initialized")
	
	# Wait one frame to ensure all nodes are ready
	await get_tree().process_frame
	
	# Initialize UI
	if stamina_bar:
		stamina_bar.min_value = 0
		stamina_bar.max_value = 100
		stamina_bar.value = 100
		print("Stamina bar initialized")
	
	# Generate world
	print("Starting world generation")
	generate_world()
	print("World generation completed")

func generate_world():
	var params = {
		"seed": randi()  # Random seed for world generation
	}
	
	if !world_system:
		push_error("World system not found!")
		return
		
	print("Generating world with seed: ", params.seed)
	world_system.generate_world(params)
	
	# Spawn player in first room
	var spawn_pos = world_system.get_first_room_center()
	if spawn_pos != Vector2.ZERO:
		if player and player is CharacterBody2D:
			player.position = spawn_pos
			print("Player spawned at: ", spawn_pos)
		else:
			push_error("Player node not found or invalid type!")
	else:
		push_error("Failed to find valid player spawn position")

func _on_player_stamina_changed(value: float):
	if stamina_bar:
		stamina_bar.value = value * 100 
