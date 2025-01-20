#ifndef REALITY_SYSTEM_H
#define REALITY_SYSTEM_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <vector>
#include <map>
#include <string>

namespace godot {

// Forward declarations
class PersonalityProfile;

// Reality distortion effects
enum class DistortionEffect {
    NONE,
    RIPPLE,         // Reality ripples outward from a point
    FRAGMENT,       // Reality breaks into fragments
    MERGE,          // Multiple realities merge
    DISSOLVE,       // Reality temporarily dissolves
    CRYSTALLIZE     // Reality becomes more "real" than real
};

// Psychological influence types
enum class InfluenceType {
    EMOTIONAL,      // Direct emotional impact
    COGNITIVE,      // Affects thought patterns
    PERCEPTUAL,     // Changes how reality is perceived
    BEHAVIORAL,     // Modifies behavior patterns
    COLLECTIVE      // Affects group consciousness
};

// Structure for tracking reality anomalies
struct RealityAnomaly {
    Vector3 position;
    float intensity;
    float radius;
    DistortionEffect effect;
    Dictionary properties;
    bool is_active;
    float duration;
    float elapsed_time;
};

// Structure for consciousness fields
struct ConsciousnessField {
    std::vector<PersonalityProfile*> affected_entities;
    InfluenceType influence_type;
    float field_strength;
    Vector3 origin;
    float radius;
    Dictionary influence_properties;
};

// Main reality system class
class RealitySystem : public Node3D {
    GDCLASS(RealitySystem, Node3D)

private:
    // Reality state tracking
    std::vector<RealityAnomaly> active_anomalies;
    std::vector<ConsciousnessField> consciousness_fields;
    float global_distortion_level;
    Dictionary reality_state;
    
    // Influence tracking
    std::map<InfluenceType, float> influence_thresholds;
    std::vector<PersonalityProfile*> influenced_entities;
    
    // Internal methods
    void update_anomalies(float delta);
    void process_consciousness_fields(float delta);
    void calculate_reality_state();
    void apply_influence_effects();
    void cleanup_expired_effects();

protected:
    static void _bind_methods();

public:
    RealitySystem();
    ~RealitySystem();

    // Core functionality
    void _process(double delta) override;
    void _physics_process(double delta) override;
    
    // Reality manipulation
    void create_reality_anomaly(const Vector3& position, DistortionEffect effect, float intensity);
    void remove_reality_anomaly(const Vector3& position);
    bool is_position_distorted(const Vector3& position) const;
    float get_distortion_intensity(const Vector3& position) const;
    
    // Consciousness field management
    void create_consciousness_field(const Vector3& origin, InfluenceType type, float strength);
    void update_consciousness_field(int field_id, const Dictionary& properties);
    void remove_consciousness_field(int field_id);
    
    // Entity influence
    void register_entity(PersonalityProfile* entity);
    void unregister_entity(PersonalityProfile* entity);
    void apply_influence(PersonalityProfile* target, InfluenceType type, float strength);
    
    // State queries
    Dictionary get_reality_state() const;
    Array get_active_anomalies() const;
    Array get_consciousness_fields() const;
    float get_global_distortion() const;
    
    // Configuration
    void set_influence_threshold(InfluenceType type, float threshold);
    float get_influence_threshold(InfluenceType type) const;
    void set_global_distortion(float level);
};

}

#endif // REALITY_SYSTEM_H 