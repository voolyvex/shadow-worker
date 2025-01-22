extends CharacterBody2D

@export var speed: float = 300.0
@export var sprint_multiplier: float = 1.5
@export var interaction_range: float = 50.0

var current_npc: CharacterBody2D = null
var is_sprinting: bool = false
var initial_position: Vector2

@onready var stamina_bar = $"../UI/StaminaBar"
@onready var camera = $Camera2D

func _ready() -> void:
	# Store initial position for reference
	initial_position = position
	
	# Ensure camera is properly configured
	if camera:
		camera.make_current()
		camera.reset_smoothing()

func _physics_process(delta: float) -> void:
	# Get input direction
	var direction := Vector2.ZERO
	direction.x = Input.get_axis("ui_left", "ui_right")
	direction.y = Input.get_axis("ui_up", "ui_down")
	direction = direction.normalized()
	
	# Handle sprinting
	is_sprinting = Input.is_action_pressed("sprint") and stamina_bar.value > 0
	var current_speed = speed * (sprint_multiplier if is_sprinting else 1.0)
	
	# Update stamina
	if stamina_bar:
		if is_sprinting and direction != Vector2.ZERO:
			stamina_bar.value = max(0, stamina_bar.value - delta * 25)
		else:
			stamina_bar.value = min(100, stamina_bar.value + delta * 10)
	
	# Apply movement
	if direction:
		velocity = direction * current_speed
	else:
		velocity = velocity.move_toward(Vector2.ZERO, current_speed)
	
	# Use move_and_slide for proper collision handling
	move_and_slide()
	
	# Handle NPC interaction
	if Input.is_action_just_pressed("interact"):
		var space_state = get_world_2d().direct_space_state
		var query = PhysicsRayQueryParameters2D.create(global_position, global_position + Vector2.RIGHT.rotated(rotation) * interaction_range)
		query.collision_mask = 2  # NPC layer
		var result = space_state.intersect_ray(query)
		
		if result and result.collider.has_method("start_interaction"):
			result.collider.start_interaction()
			current_npc = result.collider
		elif current_npc:
			current_npc.end_interaction()
			current_npc = null

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("escape") and current_npc:
		current_npc.end_interaction()
		current_npc = null
