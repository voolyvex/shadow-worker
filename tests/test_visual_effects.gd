extends Node2D

const MockVisualEffectsManager = preload("res://tests/mock_visual_effects.gd")

signal test_completed(test_name)
signal test_failed(test_name, message)

var effects_manager: MockVisualEffectsManager

func _ready():
    effects_manager = $"../VisualEffects"
    if not effects_manager:
        effects_manager = MockVisualEffectsManager.new()
        add_child(effects_manager)

func run_all_tests():
    print("Running Visual Effects Tests...")
    
    effects_manager.reset()
    await test_effect_start()
    
    effects_manager.reset()
    await test_effect_stop()
    
    effects_manager.reset()
    await test_global_intensity()
    
    effects_manager.reset()
    await test_multiple_effects()
    
    print("Visual Effects Tests completed.")

func test_effect_start():
    var test_name = "test_effect_start"
    print("Running " + test_name)
    
    var effect_name = "ripple_test"
    var params = {
        "intensity": 0.5,
        "duration": 2.0,
        "position": Vector2(100, 100)
    }
    
    var success = effects_manager.start_effect(effect_name, params)
    var active_effects = effects_manager.get_active_effects()
    
    if not success:
        emit_signal("test_failed", test_name, "Failed to start effect")
        return
    
    if not active_effects.has(effect_name):
        emit_signal("test_failed", test_name, "Effect not found in active effects")
        return
        
    if active_effects[effect_name].intensity != params.intensity:
        emit_signal("test_failed", test_name, "Effect intensity mismatch")
        return
    
    emit_signal("test_completed", test_name)

func test_effect_stop():
    var test_name = "test_effect_stop"
    print("Running " + test_name)
    
    var effect_name = "test_effect"
    effects_manager.start_effect(effect_name, {"intensity": 0.7})
    
    var success = effects_manager.stop_effect(effect_name)
    var active_effects = effects_manager.get_active_effects()
    
    if not success:
        emit_signal("test_failed", test_name, "Failed to stop effect")
        return
    
    if active_effects.has(effect_name):
        emit_signal("test_failed", test_name, "Effect still present in active effects")
        return
    
    emit_signal("test_completed", test_name)

func test_global_intensity():
    var test_name = "test_global_intensity"
    print("Running " + test_name)
    
    var test_intensity = 0.75
    effects_manager.set_global_intensity(test_intensity)
    
    var current_intensity = effects_manager.get_global_intensity()
    
    if current_intensity != test_intensity:
        emit_signal("test_failed", test_name, "Global intensity mismatch")
        return
    
    emit_signal("test_completed", test_name)

func test_multiple_effects():
    var test_name = "test_multiple_effects"
    print("Running " + test_name)
    
    effects_manager.start_effect("effect1", {"intensity": 0.3})
    effects_manager.start_effect("effect2", {"intensity": 0.5})
    effects_manager.start_effect("effect3", {"intensity": 0.7})
    
    var active_effects = effects_manager.get_active_effects()
    
    if active_effects.size() != 3:
        emit_signal("test_failed", test_name, "Expected 3 active effects, got " + str(active_effects.size()))
        return
    
    emit_signal("test_completed", test_name) 