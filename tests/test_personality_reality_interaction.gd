extends Node2D

const Constants = preload("res://tests/test_constants.gd")

var reality_system
var test_npc

signal test_completed(test_name)
signal test_failed(test_name, message)

func _ready():
    reality_system = $RealitySystem
    test_npc = $TestNPC
    
    if not reality_system:
        push_error("RealitySystem node not found")
        return
        
    if not test_npc:
        push_error("TestNPC node not found")
        return

func run_all_tests():
    print("Running Personality-Reality Interaction Tests...")
    
    await test_reality_influence_on_personality()
    await test_personality_influence_on_reality()
    await test_npc_reality_interaction()
    await test_emotional_resonance()
    await test_cosmic_insight()
    
    print("Personality-Reality Interaction Tests completed.")

func test_reality_influence_on_personality():
    var test_name = "test_reality_influence_on_personality"
    print("Running " + test_name)
    
    # Create a reality anomaly
    var anomaly_pos = Vector2(100, 100)
    reality_system.create_reality_anomaly(anomaly_pos, Constants.DistortionEffect.RIPPLE, 0.7)
    
    # Move NPC near anomaly
    test_npc.position = anomaly_pos + Vector2(10, 10)
    
    # Wait for influence to take effect
    await get_tree().create_timer(1.0).timeout
    
    # Check personality changes
    var personality_state = test_npc.get_personality_state()
    
    if personality_state["cosmic_awareness"] <= 0.0:
        emit_signal("test_failed", test_name, "Expected increased cosmic awareness")
        return
        
    if personality_state["stress_level"] <= 0.0:
        emit_signal("test_failed", test_name, "Expected increased stress level")
        return
    
    emit_signal("test_completed", test_name)

func test_personality_influence_on_reality():
    var test_name = "test_personality_influence_on_reality"
    print("Running " + test_name)
    
    # Set high cosmic awareness
    test_npc.set_personality_trait("cosmic_awareness", 0.9)
    
    # Wait for influence to take effect
    await get_tree().create_timer(1.0).timeout
    
    # Check reality distortion
    var distortion = reality_system.get_distortion_intensity(test_npc.position)
    
    if distortion <= 0.0:
        emit_signal("test_failed", test_name, "Expected reality distortion around NPC")
        return
    
    emit_signal("test_completed", test_name)

func test_npc_reality_interaction():
    var test_name = "test_npc_reality_interaction"
    print("Running " + test_name)
    
    # Create multiple reality anomalies
    reality_system.create_reality_anomaly(Vector2(100, 100), Constants.DistortionEffect.RIPPLE, 0.5)
    reality_system.create_reality_anomaly(Vector2(200, 200), Constants.DistortionEffect.FRAGMENT, 0.7)
    
    # Move NPC between anomalies
    test_npc.position = Vector2(150, 150)
    
    # Wait for interactions
    await get_tree().create_timer(1.0).timeout
    
    # Check NPC state changes
    var npc_state = test_npc.get_personality_state()
    
    if npc_state["stress_level"] <= 0.3:
        emit_signal("test_failed", test_name, "Expected high stress from multiple anomalies")
        return
        
    if npc_state["cosmic_awareness"] <= 0.4:
        emit_signal("test_failed", test_name, "Expected increased cosmic awareness from exposure")
        return
    
    emit_signal("test_completed", test_name)

func test_emotional_resonance():
    var test_name = "test_emotional_resonance"
    print("Running " + test_name)
    
    # Create consciousness field
    reality_system.create_consciousness_field(test_npc.position, Constants.InfluenceType.EMOTIONAL, 0.8)
    
    # Wait for resonance
    await get_tree().create_timer(1.0).timeout
    
    # Check emotional state
    var emotional_state = test_npc.get_personality_state()["emotional_state"]
    
    if not emotional_state.has("resonance"):
        emit_signal("test_failed", test_name, "Expected emotional resonance state")
        return
        
    if emotional_state["resonance"] <= 0.0:
        emit_signal("test_failed", test_name, "Expected active emotional resonance")
        return
    
    emit_signal("test_completed", test_name)

func test_cosmic_insight():
    var test_name = "test_cosmic_insight"
    print("Running " + test_name)
    
    # Set high cosmic awareness
    test_npc.set_personality_trait("cosmic_awareness", 1.0)
    
    # Create reality anomaly
    reality_system.create_reality_anomaly(test_npc.position, Constants.DistortionEffect.CRYSTALLIZE, 0.9)
    
    # Wait for insight processing
    await get_tree().create_timer(1.0).timeout
    
    # Check insight state
    var insight_level = test_npc.get_insight_level()
    
    if insight_level < 0.8:
        emit_signal("test_failed", test_name, "Expected high insight level")
        return
        
    if not test_npc.has_eyes_on_the_inside():
        emit_signal("test_failed", test_name, "Expected transcendent state")
        return
    
    emit_signal("test_completed", test_name) 