extends CharacterBody2D

signal stamina_changed(value)
signal interaction_started(target)

const WALK_SPEED = 200.0
const SPRINT_SPEED = 400.0
const STAMINA_MAX = 100.0
const STAMINA_DRAIN_RATE = 30.0
const STAMINA_REGEN_RATE = 20.0
const SPRINT_THRESHOLD = 20.0

var current_speed = WALK_SPEED
var stamina = STAMINA_MAX
var is_sprinting = false
var interaction_target = null

@onready var interaction_ray = $InteractionRay

func _ready():
	add_to_group("player")
	stamina_changed.emit(stamina / STAMINA_MAX)

func _physics_process(delta):
	var input_dir = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	
	# Handle sprinting
	if Input.is_action_pressed("sprint") and stamina > SPRINT_THRESHOLD:
		is_sprinting = true
		current_speed = SPRINT_SPEED
		stamina = max(0, stamina - STAMINA_DRAIN_RATE * delta)
		stamina_changed.emit(stamina / STAMINA_MAX)
	else:
		is_sprinting = false
		current_speed = WALK_SPEED
		if stamina < STAMINA_MAX:
			stamina = min(STAMINA_MAX, stamina + STAMINA_REGEN_RATE * delta)
			stamina_changed.emit(stamina / STAMINA_MAX)
	
	# Apply movement
	if input_dir:
		velocity = input_dir * current_speed
	else:
		velocity = velocity.move_toward(Vector2.ZERO, current_speed)
	
	move_and_slide()
	
	# Handle interaction
	if Input.is_action_just_pressed("interact"):
		start_interaction()
	elif Input.is_action_just_pressed("cancel_interaction"):
		end_interaction()

func start_interaction():
	if interaction_ray.is_colliding():
		var collider = interaction_ray.get_collider()
		if collider.is_in_group("interactable"):
			interaction_target = collider
			interaction_started.emit(interaction_target)

func end_interaction():
	interaction_target = null

func get_stamina():
	return stamina / STAMINA_MAX
