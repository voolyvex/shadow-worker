extends StaticBody2D

signal interaction_started
signal interaction_ended

@onready var interaction_label = $Label
@onready var interaction_area = $InteractionArea

var can_interact = true
var player_in_range = false

func _ready():
    interaction_area.connect("body_entered", _on_interaction_area_body_entered)
    interaction_area.connect("body_exited", _on_interaction_area_body_exited)

func _on_interaction_area_body_entered(body):
    if body.is_in_group("player"):
        player_in_range = true
        if can_interact:
            interaction_label.show()
            emit_signal("interaction_started")

func _on_interaction_area_body_exited(body):
    if body.is_in_group("player"):
        player_in_range = false
        interaction_label.hide()
        emit_signal("interaction_ended")

func interact():
    if can_interact and player_in_range:
        _handle_interaction()

func _handle_interaction():
    # Override this method in child classes
    pass

func set_interaction_enabled(enabled: bool):
    can_interact = enabled
    if not can_interact:
        interaction_label.hide()

func set_interaction_text(text: String):
    interaction_label.text = text 