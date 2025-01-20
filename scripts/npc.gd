extends Node2D

signal interaction_started
signal interaction_ended

# Debug variables
@onready var debug_label = Label.new()
var show_debug = true
var player_in_range = false

func _ready() -> void:
	var area = $Area2D
	area.body_entered.connect(_on_body_entered)
	area.body_exited.connect(_on_body_exited)
	
	# Setup debug label
	if show_debug:
		debug_label.position = Vector2(0, -40)
		add_child(debug_label)
		_update_debug_text()

func _on_body_entered(body: Node2D) -> void:
	if body.is_in_group("player"):
		player_in_range = true
		interaction_started.emit()
		_update_debug_text()

func _on_body_exited(body: Node2D) -> void:
	if body.is_in_group("player"):
		player_in_range = false
		interaction_ended.emit()
		_update_debug_text()

func _update_debug_text() -> void:
	if show_debug:
		debug_label.text = "Interaction: %s" % ["Active" if player_in_range else "Inactive"]

func _input(event: InputEvent) -> void:
	# Toggle debug display with F3
	if event.is_action_pressed("toggle_debug"):
		show_debug = !show_debug
		debug_label.visible = show_debug
