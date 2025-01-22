extends CharacterBody2D

enum NPCState { IDLE, PATROL, INTERACT }

@export var base_speed: float = 100.0
@export var patrol_radius: float = 200.0
@export var idle_time_range: Vector2 = Vector2(2.0, 5.0)
@export_enum("HELPER", "MERCHANT", "SAGE", "TRICKSTER") var personality_type: String = "HELPER"

var current_state: NPCState = NPCState.IDLE
var start_position: Vector2
var target_position: Vector2
var idle_timer: float = 0.0
var current_idle_duration: float = 0.0
var is_interacting: bool = false
var dialogue_text: String = ""

var dialogue_options = {
	"HELPER": [
		"Need any help? I know these halls well!",
		"Be careful of the shadows ahead...",
		"I can show you the safest paths if you'd like."
	],
	"MERCHANT": [
		"Got some rare items for sale, if you're interested!",
		"Everything has a price, friend...",
		"I've traveled far to bring these wares."
	],
	"SAGE": [
		"The ancient texts speak of great power hidden here...",
		"Knowledge is the key to survival in these depths.",
		"Seek wisdom, and you shall find your path."
	],
	"TRICKSTER": [
		"Hehe, watch your step! Or don't, could be fun!",
		"Want to hear a secret? Maybe it's true, maybe not!",
		"Things aren't always what they seem around here..."
	]
}

@onready var dialogue_label: Label = $DialogueLabel
@onready var debug_label: Label = $DebugLabel
@onready var sprite: Sprite2D = $Sprite2D

func _ready() -> void:
	start_position = position
	target_position = start_position
	dialogue_label.text = ""
	debug_label.text = personality_type
	add_to_group("interactable")
	_set_random_dialogue()
	_enter_idle_state()

func _physics_process(delta: float) -> void:
	if is_interacting:
		return
		
	match current_state:
		NPCState.IDLE:
			_process_idle_state(delta)
		NPCState.PATROL:
			_process_patrol_state(delta)
		NPCState.INTERACT:
			_process_interact_state(delta)

func _process_idle_state(delta: float) -> void:
	idle_timer += delta
	if idle_timer >= current_idle_duration:
		_enter_patrol_state()

func _process_patrol_state(delta: float) -> void:
	var direction = target_position - position
	
	if direction.length() < 5:
		_enter_idle_state()
	else:
		direction = direction.normalized()
		velocity = direction * base_speed
		move_and_slide()
		_update_sprite_direction(direction)

func _process_interact_state(_delta: float) -> void:
	# Handle interaction-specific behavior
	pass

func _enter_idle_state() -> void:
	current_state = NPCState.IDLE
	velocity = Vector2.ZERO
	idle_timer = 0.0
	current_idle_duration = randf_range(idle_time_range.x, idle_time_range.y)

func _enter_patrol_state() -> void:
	current_state = NPCState.PATROL
	var angle = randf() * 2 * PI
	var distance = randf() * patrol_radius
	target_position = start_position + Vector2(cos(angle), sin(angle)) * distance

func _update_sprite_direction(direction: Vector2) -> void:
	if sprite and direction.x != 0:
		sprite.flip_h = direction.x < 0

func start_interaction() -> void:
	is_interacting = true
	current_state = NPCState.INTERACT
	_set_random_dialogue()
	dialogue_label.text = dialogue_text
	debug_label.hide()

func end_interaction() -> void:
	is_interacting = false
	current_state = NPCState.IDLE
	dialogue_label.text = ""
	debug_label.show()
	debug_label.text = personality_type
	_enter_idle_state()

func _set_random_dialogue() -> void:
	var options = dialogue_options[personality_type]
	dialogue_text = options[randi() % options.size()]

func set_dialogue(text: String) -> void:
	dialogue_text = text
	if is_interacting:
		dialogue_label.text = dialogue_text

func set_personality(type: String) -> void:
	if type in dialogue_options:
		personality_type = type
		_set_random_dialogue()
		if not is_interacting:
			debug_label.text = type

# Signal handlers for area detection (can be used for player proximity)
func _on_detection_area_entered(area: Area2D) -> void:
	if area.is_in_group("player"):
		debug_label.show()

func _on_detection_area_exited(area: Area2D) -> void:
	if area.is_in_group("player"):
		debug_label.hide()
