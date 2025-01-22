extends CharacterBody2D
class_name MockNPC

var personality_state = {
	"cosmic_awareness": 0.0,
	"stress_level": 0.0,
	"emotional_state": {},
	"insight_level": 0.0
}

func _ready():
	# Initialize empty emotional state
	personality_state["emotional_state"] = {}
	# Get reference to reality system and register self
	var reality_system = get_node("../RealitySystem")
	if reality_system:
		reality_system.register_npc(self)

func get_personality_state() -> Dictionary:
	return personality_state

func set_personality_trait(trait_name: String, value: float) -> void:
	personality_state[trait_name] = value
	if trait_name == "cosmic_awareness":
		personality_state["insight_level"] = value * 0.9
		if value >= 0.5:
			personality_state["emotional_state"]["resonance"] = value * 0.8

func get_insight_level() -> float:
	return personality_state["insight_level"]

func has_eyes_on_the_inside() -> bool:
	return personality_state["cosmic_awareness"] >= 0.8 
