@tool
extends Node
class_name MockRealitySystem

var anomalies = []
var consciousness_fields = []

func _init():
    reset()

func reset():
    anomalies.clear()
    consciousness_fields.clear()

func create_anomaly(position: Vector2, effect: int, intensity: float) -> bool:
    anomalies.append({
        "position": position,
        "effect": effect,
        "intensity": intensity
    })
    return true

func remove_anomaly(position: Vector2) -> bool:
    for i in range(anomalies.size()):
        if anomalies[i].position == position:
            anomalies.remove_at(i)
            return true
    return false

func create_consciousness_field(position: Vector2, radius: float, influence_type: int) -> bool:
    consciousness_fields.append({
        "position": position,
        "radius": radius,
        "influence_type": influence_type
    })
    return true

func get_anomalies() -> Array:
    return anomalies

func get_consciousness_fields() -> Array:
    return consciousness_fields 