@tool
extends Node
class_name MockRealitySystem

const Constants = preload("res://tests/test_constants.gd")

var anomalies = []
var consciousness_fields = []
var global_distortion = 0.0
var affected_npcs = []

func _ready():
    reset()

func _process(_delta):
    # Process anomaly effects on NPCs
    for npc in affected_npcs:
        var total_influence = 0.0
        var total_stress = 0.0
        
        # Calculate influence from anomalies
        for anomaly in anomalies:
            var distance = npc.position.distance_to(anomaly.position)
            if distance < 50.0:
                var influence = anomaly.intensity * (1.0 - distance / 50.0)
                total_influence += influence
                total_stress += influence * 0.5
        
        # Apply effects to NPC
        if total_influence > 0:
            var state = npc.get_personality_state()
            state["cosmic_awareness"] = min(state["cosmic_awareness"] + total_influence * 0.1, 1.0)
            state["stress_level"] = min(state["stress_level"] + total_stress, 1.0)
        
        # Process consciousness fields
        for field in consciousness_fields:
            var distance = npc.position.distance_to(field.position)
            if distance < 50.0:
                var influence = field.strength * (1.0 - distance / 50.0)
                if field.type == Constants.InfluenceType.EMOTIONAL:
                    npc.personality_state["emotional_state"]["resonance"] = influence

func reset():
    anomalies.clear()
    consciousness_fields.clear()
    global_distortion = 0.0
    affected_npcs.clear()

func register_npc(npc):
    if not npc in affected_npcs:
        affected_npcs.append(npc)

func create_reality_anomaly(position: Vector2, effect: int, intensity: float) -> void:
    anomalies.append({
        "position": position,
        "effect": effect,
        "intensity": intensity
    })
    global_distortion += intensity * 0.1

func remove_reality_anomaly(position: Vector2) -> void:
    for i in range(anomalies.size()):
        if anomalies[i].position.distance_to(position) < 0.1:
            global_distortion = max(global_distortion - anomalies[i].intensity * 0.1, 0.0)
            anomalies.remove_at(i)
            break

func get_distortion_intensity(position: Vector2) -> float:
    var total_intensity = 0.0
    for anomaly in anomalies:
        var distance = position.distance_to(anomaly.position)
        if distance < 50.0:  # Influence radius
            total_intensity += anomaly.intensity * (1.0 - distance / 50.0)
    return total_intensity

func create_consciousness_field(position: Vector2, radius: float, influence_type: int) -> void:
    consciousness_fields.append({
        "position": position,
        "radius": radius,
        "influence_type": influence_type,
        "strength": 1.0  # Default strength
    })

func get_anomalies() -> Array:
    return anomalies

func get_consciousness_fields() -> Array:
    return consciousness_fields

func create_anomaly(position: Vector2, effect: int, intensity: float) -> bool:
    create_reality_anomaly(position, effect, intensity)
    return true

func remove_anomaly(position: Vector2) -> bool:
    remove_reality_anomaly(position)
    return true 