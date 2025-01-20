extends CharacterBody2D

signal interaction_started(npc)
signal interaction_ended(npc)

var dialogue_text = "Hello!"
var is_interacting = false

@onready var dialogue_display = $DialogueDisplay

# Debug variables
@onready var debug_label = Label.new()
var show_debug = true
var player_in_range = false

func _ready():
	dialogue_display.text = ""
	add_to_group("interactable")
	
	var area = $DetectionArea
	area.body_entered.connect(_on_body_entered)
	area.body_exited.connect(_on_body_exited)
	
	# Setup debug label
	if show_debug:
		debug_label.position = Vector2(0, -40)
		add_child(debug_label)
		_update_debug_text()

func start_interaction():
	if not is_interacting:
		is_interacting = true
		dialogue_display.text = dialogue_text
		interaction_started.emit(self)

func end_interaction():
	if is_interacting:
		is_interacting = false
		dialogue_display.text = ""
		interaction_ended.emit(self)

func set_dialogue(text: String):
	dialogue_text = text
	if is_interacting:
		dialogue_display.text = dialogue_text

func _on_body_entered(body: Node2D) -> void:
	if body.is_in_group("player"):
		player_in_range = true
		interaction_started.emit(self)
		_update_debug_text()

func _on_body_exited(body: Node2D) -> void:
	if body.is_in_group("player"):
		player_in_range = false
		interaction_ended.emit(self)
		_update_debug_text()

func _update_debug_text() -> void:
	if show_debug:
		debug_label.text = "Interaction: %s" % ["Active" if player_in_range else "Inactive"]

func _input(event: InputEvent) -> void:
	# Toggle debug display with F3
	if event.is_action_pressed("toggle_debug"):
		show_debug = !show_debug
		debug_label.visible = show_debug
