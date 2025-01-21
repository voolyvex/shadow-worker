extends SceneTree

func _init():
    print("Starting test suite...")
    
    var test_scene = load("res://tests/test_personality_reality_interaction.tscn").instantiate()
    root.add_child(test_scene)
    
    test_scene.test_completed.connect(_on_test_completed)
    test_scene.test_failed.connect(_on_test_failed)
    
    await test_scene.run_all_tests()
    
    print("\nAll tests completed.")
    quit()

func _on_test_completed(test_name: String):
    print("✓ " + test_name + " passed")

func _on_test_failed(test_name: String, message: String):
    print("✗ " + test_name + " failed: " + message)
    quit(1) 