extends CharacterBody2D

signal interaction_started(npc)
signal interaction_ended(npc)

# Personality and behavior configuration
@export var personality_type: String = "HELPER"  # Enneagram type
@export var base_speed: float = 100.0
@export var patrol_radius: float = 200.0
@export var interaction_radius: float = 50.0

# Internal state
var npc_controller = null  # C++ NPC controller
var dialogue_text = "Hello!"
var is_interacting = false
var debug_enabled = false

# Node references
@onready var dialogue_display = $DialogueDisplay
@onready var detection_area = $DetectionArea
@onready var sprite = $AnimatedSprite2D
@onready var debug_label = $DebugLabel

func _ready():
	# Initialize C++ controller
	npc_controller = NPC.new()
	add_child(npc_controller)
	
	# Configure controller
	npc_controller.set_speed(base_speed)
	npc_controller.set_patrol_radius(patrol_radius)
	npc_controller.set_interaction_radius(interaction_radius)
	npc_controller.initialize_personality(personality_type)
	
	# Setup initial state
	dialogue_display.text = ""
	add_to_group("interactable")
	
	# Connect signals
	detection_area.body_entered.connect(_on_body_entered)
	detection_area.body_exited.connect(_on_body_exited)
	
	# Initialize debug display
	setup_debug_display()

func _process(delta):
	if npc_controller:
		# Update debug info if enabled
		if debug_enabled:
			update_debug_display()

func _physics_process(delta):
	if npc_controller:
		# Apply velocity from C++ controller
		velocity = npc_controller.get_velocity()
		move_and_slide()
		
		# Update sprite direction
		if velocity.x != 0:
			sprite.flip_h = velocity.x < 0

func start_interaction():
	if not is_interacting and npc_controller:
		is_interacting = true
		npc_controller.on_interaction_start()
		dialogue_display.text = dialogue_text
		interaction_started.emit(self)

func end_interaction():
	if is_interacting and npc_controller:
		is_interacting = false
		npc_controller.on_interaction_end()
		dialogue_display.text = ""
		interaction_ended.emit(self)

func set_dialogue(text: String):
	dialogue_text = text
	if is_interacting:
		dialogue_display.text = dialogue_text
		if npc_controller:
			npc_controller.queue_dialogue(text)

func _on_body_entered(body: Node2D) -> void:
	if body.is_in_group("player"):
		if npc_controller:
			npc_controller.on_player_entered(body)
		interaction_started.emit(self)
		update_debug_display()

func _on_body_exited(body: Node2D) -> void:
	if body.is_in_group("player"):
		if npc_controller:
			npc_controller.on_player_exited(body)
		interaction_ended.emit(self)
		update_debug_display()

func setup_debug_display() -> void:
	if not debug_label:
		var label = Label.new()
		label.name = "DebugLabel"
		label.position = Vector2(0, -40)
		add_child(label)
		debug_label = label
	debug_label.visible = debug_enabled

func update_debug_display() -> void:
	if debug_enabled and debug_label and npc_controller:
		var state = npc_controller.get_state()
		var personality = npc_controller.get_personality_state()
		var text = "State: %s\n" % state
		text += "Personality: %s\n" % personality
		text += "Interaction: %s" % ["Active" if is_interacting else "Inactive"]
		debug_label.text = text

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("toggle_debug"):
		debug_enabled = !debug_enabled
		debug_label.visible = debug_enabled
		update_debug_display()

func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE:
		# Clean up C++ controller
		if npc_controller:
			npc_controller.queue_free()
			npc_controller = null
