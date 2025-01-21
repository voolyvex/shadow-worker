extends SceneTree

const Constants = preload("res://tests/test_constants.gd")

var total_tests = 0
var passed_tests = 0
var failed_tests = 0
var exit_code = 0

func _init():
    print("Starting test suite in headless mode...")
    
    # Initialize constants
    var constants = Constants.new()
    get_root().add_child(constants)
    
    var test_scene = load("res://tests/test_scene.tscn").instantiate()
    get_root().add_child(test_scene)
    
    # Give nodes a chance to initialize
    await get_root().ready
    
    var reality_tests = test_scene.get_node("RealityTests")
    var visual_tests = test_scene.get_node("VisualTests")
    
    reality_tests.test_completed.connect(_on_test_completed)
    reality_tests.test_failed.connect(_on_test_failed)
    visual_tests.test_completed.connect(_on_test_completed)
    visual_tests.test_failed.connect(_on_test_failed)
    
    print("Running Reality System Tests...")
    await reality_tests.run_all_tests()
    
    print("Running Visual Effects Tests...")
    await visual_tests.run_all_tests()
    
    print_summary()
    quit(exit_code)

func _on_test_completed(test_name: String):
    print("Test passed: " + test_name)
    passed_tests += 1
    total_tests += 1

func _on_test_failed(test_name: String, message: String):
    print("Test failed: " + test_name + " - " + message)
    failed_tests += 1
    total_tests += 1
    exit_code = 1

func print_summary():
    print("\nTest Summary:")
    print("Total tests: " + str(total_tests))
    print("Passed: " + str(passed_tests))
    print("Failed: " + str(failed_tests)) 