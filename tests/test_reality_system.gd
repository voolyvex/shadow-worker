extends Node2D

const MockRealitySystem = preload("res://tests/mock_reality_system.gd")

signal test_completed(test_name)
signal test_failed(test_name, message)

var reality_system: MockRealitySystem

func _ready():
    reality_system = $"../RealitySystem"
    if not reality_system:
        reality_system = MockRealitySystem.new()
        add_child(reality_system)

func run_all_tests():
    print("Running Reality System Tests...")
    
    reality_system.reset()
    await test_anomaly_creation()
    
    reality_system.reset()
    await test_anomaly_removal()
    
    reality_system.reset()
    await test_consciousness_field_creation()
    
    reality_system.reset()
    await test_multiple_anomalies()
    
    reality_system.reset()
    await test_multiple_consciousness_fields()
    
    print("Reality System Tests completed.")

func test_anomaly_creation():
    var test_name = "test_anomaly_creation"
    print("Running " + test_name)
    
    var position = Vector2(100, 100)
    var effect = Constants.DistortionEffect.RIPPLE
    var intensity = 0.5
    
    var success = reality_system.create_anomaly(position, effect, intensity)
    var anomalies = reality_system.get_anomalies()
    
    if not success:
        emit_signal("test_failed", test_name, "Failed to create anomaly")
        return
    
    if anomalies.size() != 1:
        emit_signal("test_failed", test_name, "Expected 1 anomaly, got " + str(anomalies.size()))
        return
        
    if anomalies[0].position != position:
        emit_signal("test_failed", test_name, "Anomaly position mismatch")
        return
        
    if anomalies[0].effect != effect:
        emit_signal("test_failed", test_name, "Anomaly effect mismatch")
        return
        
    if anomalies[0].intensity != intensity:
        emit_signal("test_failed", test_name, "Anomaly intensity mismatch")
        return
    
    emit_signal("test_completed", test_name)

func test_anomaly_removal():
    var test_name = "test_anomaly_removal"
    print("Running " + test_name)
    
    var position = Vector2(200, 200)
    reality_system.create_anomaly(position, Constants.DistortionEffect.FRAGMENT, 0.7)
    
    var success = reality_system.remove_anomaly(position)
    var anomalies = reality_system.get_anomalies()
    
    if not success:
        emit_signal("test_failed", test_name, "Failed to remove anomaly")
        return
    
    if anomalies.size() != 0:
        emit_signal("test_failed", test_name, "Expected 0 anomalies, got " + str(anomalies.size()))
        return
    
    emit_signal("test_completed", test_name)

func test_consciousness_field_creation():
    var test_name = "test_consciousness_field_creation"
    print("Running " + test_name)
    
    var position = Vector2(300, 300)
    var radius = 50.0
    var influence_type = Constants.InfluenceType.EMOTIONAL
    
    var success = reality_system.create_consciousness_field(position, radius, influence_type)
    var fields = reality_system.get_consciousness_fields()
    
    if not success:
        emit_signal("test_failed", test_name, "Failed to create consciousness field")
        return
    
    if fields.size() != 1:
        emit_signal("test_failed", test_name, "Expected 1 field, got " + str(fields.size()))
        return
        
    if fields[0].position != position:
        emit_signal("test_failed", test_name, "Field position mismatch")
        return
        
    if fields[0].radius != radius:
        emit_signal("test_failed", test_name, "Field radius mismatch")
        return
        
    if fields[0].influence_type != influence_type:
        emit_signal("test_failed", test_name, "Field influence type mismatch")
        return
    
    emit_signal("test_completed", test_name)

func test_multiple_anomalies():
    var test_name = "test_multiple_anomalies"
    print("Running " + test_name)
    
    reality_system.create_anomaly(Vector2(100, 100), Constants.DistortionEffect.RIPPLE, 0.5)
    reality_system.create_anomaly(Vector2(200, 200), Constants.DistortionEffect.FRAGMENT, 0.7)
    reality_system.create_anomaly(Vector2(300, 300), Constants.DistortionEffect.DISSOLVE, 0.3)
    
    var anomalies = reality_system.get_anomalies()
    
    if anomalies.size() != 3:
        emit_signal("test_failed", test_name, "Expected 3 anomalies, got " + str(anomalies.size()))
        return
    
    emit_signal("test_completed", test_name)

func test_multiple_consciousness_fields():
    var test_name = "test_multiple_consciousness_fields"
    print("Running " + test_name)
    
    reality_system.create_consciousness_field(Vector2(100, 100), 50.0, Constants.InfluenceType.EMOTIONAL)
    reality_system.create_consciousness_field(Vector2(200, 200), 75.0, Constants.InfluenceType.COGNITIVE)
    reality_system.create_consciousness_field(Vector2(300, 300), 100.0, Constants.InfluenceType.PERCEPTUAL)
    
    var fields = reality_system.get_consciousness_fields()
    
    if fields.size() != 3:
        emit_signal("test_failed", test_name, "Expected 3 fields, got " + str(fields.size()))
        return
    
    emit_signal("test_completed", test_name) 