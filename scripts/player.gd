extends CharacterBody2D

signal stamina_changed(new_stamina: float)
signal interaction_started(target: Node)
signal interaction_ended(target: Node)

const WALK_SPEED = 200.0
const SPRINT_SPEED = 400.0
const STAMINA_MAX = 100.0
const STAMINA_DRAIN_RATE = 20.0  # Per second
const STAMINA_REGEN_RATE = 10.0  # Per second
const STAMINA_SPRINT_THRESHOLD = 10.0

var current_speed = WALK_SPEED
var stamina = STAMINA_MAX
var is_sprinting = false
var current_interaction_target = null

@onready var interaction_ray = $InteractionRay

func _ready():
	# Add player to interaction group
	add_to_group("player")

func _physics_process(delta):
	# Get input direction
	var direction = Vector2.ZERO
	direction.x = Input.get_axis("ui_left", "ui_right")
	direction.y = Input.get_axis("ui_up", "ui_down")
	direction = direction.normalized()
	
	# Handle sprinting
	is_sprinting = Input.is_action_pressed("sprint") and stamina > STAMINA_SPRINT_THRESHOLD
	current_speed = SPRINT_SPEED if is_sprinting else WALK_SPEED
	
	# Update stamina
	if is_sprinting and direction != Vector2.ZERO:
		stamina = max(0.0, stamina - STAMINA_DRAIN_RATE * delta)
	elif not is_sprinting:
		stamina = min(STAMINA_MAX, stamina + STAMINA_REGEN_RATE * delta)
	
	emit_signal("stamina_changed", stamina)
	
	# Update velocity and move
	if direction:
		velocity = direction * current_speed
	else:
		velocity = velocity.move_toward(Vector2.ZERO, current_speed)
	
	move_and_slide()
	
	# Update interaction ray direction
	if direction != Vector2.ZERO:
		interaction_ray.rotation = direction.angle()
	
	# Handle interaction
	if Input.is_action_just_pressed("interact"):
		handle_interaction()

func handle_interaction():
	if interaction_ray.is_colliding():
		var target = interaction_ray.get_collider()
		if target.has_method("interact"):
			if current_interaction_target != target:
				if current_interaction_target:
					end_interaction()
				current_interaction_target = target
				emit_signal("interaction_started", target)
			target.interact(self)
	elif current_interaction_target:
		end_interaction()

func end_interaction():
	if current_interaction_target:
		emit_signal("interaction_ended", current_interaction_target)
		current_interaction_target = null

func get_stamina() -> float:
	return stamina

func _input(event):
	if event.is_action_pressed("cancel_interaction"):
		end_interaction()
