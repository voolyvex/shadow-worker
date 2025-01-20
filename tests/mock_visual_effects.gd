@tool
extends Node
class_name MockVisualEffectsManager

var active_effects = {}
var global_intensity = 0.0

func _init():
    reset()

func reset():
    active_effects.clear()
    global_intensity = 0.0

func start_effect(name: String, params: Dictionary) -> bool:
    active_effects[name] = params
    return true

func stop_effect(name: String) -> bool:
    if active_effects.has(name):
        active_effects.erase(name)
        return true
    return false

func get_active_effects() -> Dictionary:
    return active_effects

func set_global_intensity(intensity: float) -> void:
    global_intensity = intensity

func get_global_intensity() -> float:
    return global_intensity 