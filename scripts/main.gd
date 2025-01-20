extends Node2D

@onready var world_system = $WorldSystem
@onready var tilemap = $WorldSystem/TileMap
@onready var player = $Player
@onready var stamina_bar = $UI/StaminaBar

func _ready():
    # Configure world generation parameters
    world_system.set_room_size_range(6, 12)  # Min and max room dimensions
    world_system.set_room_count_range(5, 8)  # Min and max number of rooms
    world_system.set_object_density(0.15)  # Object placement density
    world_system.set_npc_scene("res://scenes/npc.tscn")
    world_system.set_npc_count_range(1, 3)  # Min and max NPCs per room
    
    # Register placeable objects
    world_system.add_placeable_object("res://scenes/objects/container.tscn", "CONTAINER", 1.0, true, true)
    world_system.add_placeable_object("res://scenes/objects/workbench.tscn", "WORKSTATION", 0.7, true, true)
    world_system.add_placeable_object("res://scenes/objects/crate.tscn", "OBSTACLE", 0.8, false, true)
    world_system.add_placeable_object("res://scenes/objects/lamp.tscn", "LIGHT_SOURCE", 0.5, true, false)
    world_system.add_placeable_object("res://scenes/objects/plant.tscn", "DECORATION", 0.3, false, false)
    
    # Generate the world with a random seed
    var seed = randi()
    world_system.generate_world(seed)
    
    # Connect player signals
    player.stamina_changed.connect(_on_player_stamina_changed)
    
    # Initialize UI
    stamina_bar.value = player.get_stamina()

func _on_player_stamina_changed(new_stamina: float):
    stamina_bar.value = new_stamina 