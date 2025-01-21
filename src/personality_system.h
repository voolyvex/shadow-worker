#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <vector>
#include <map>
#include <string>

namespace godot {

// Jungian archetypes
enum class JungianArchetype {
    PERSONA,        // Social mask
    SHADOW,         // Repressed aspects
    ANIMA,         // Feminine inner personality (in males)
    ANIMUS,        // Masculine inner personality (in females)
    SELF,          // Wholeness and unity
    WISE_OLD_MAN,  // Wisdom and guidance
    GREAT_MOTHER,  // Nurturing and destructive
    TRICKSTER,     // Chaos and disruption
    HERO           // Growth and transformation
};

// Shadow aspects that can be projected
struct ShadowAspect {
    String trait;           // The trait being projected
    float intensity;        // How strongly it's projected
    bool is_positive;       // Whether it's a positive or negative projection
    Dictionary context;     // Contextual information about when/how it manifests
};

// Dream-like surreal elements for Lynch-inspired psychological states
struct SurrealState {
    String symbol;          // Symbolic representation
    float distortion;       // Level of reality distortion
    String archetype;       // Associated archetype
    Dictionary metaphor;    // Metaphorical meaning
};

// Enneagram type definitions
enum class EnneagramType {
    REFORMER = 1,    // Type 1
    HELPER = 2,      // Type 2
    ACHIEVER = 3,    // Type 3
    INDIVIDUALIST = 4, // Type 4
    INVESTIGATOR = 5, // Type 5
    LOYALIST = 6,    // Type 6
    ENTHUSIAST = 7,  // Type 7
    CHALLENGER = 8,  // Type 8
    PEACEMAKER = 9   // Type 9
};

// DSM-5 personality dimensions with shadow integration
struct DSMDimensions {
    float negative_affectivity;
    float detachment;
    float antagonism;
    float disinhibition;
    float psychoticism;
    
    // Shadow aspects of each dimension
    std::vector<ShadowAspect> shadow_aspects;
    float shadow_integration;  // How well integrated the shadow aspects are
};

// Behavioral pattern with psychological depth
struct BehaviorPattern {
    String behavior;
    float weight;
    Dictionary context;
    JungianArchetype associated_archetype;
    std::vector<ShadowAspect> shadow_projections;
};

// Psychological resonance states
enum class ResonanceState {
    GROUNDED,       // Normal psychological state
    RESONATING,     // Beginning to affect/be affected by others
    SYNCHRONIZED,   // Deep psychological connection
    TRANSCENDENT,   // Beyond individual consciousness
    UNIFIED        // Complete integration with collective psyche
};

// Reality perception levels
enum class PerceptionLayer {
    SURFACE,        // Normal reality
    SUBCONSCIOUS,   // Awareness of deeper patterns
    COLLECTIVE,     // Access to shared unconscious
    INTEGRATED,     // Unified perception of all layers
    TRANSFORMATIVE  // Ability to affect reality through perception
};

// Inner truth manifestations
struct InnerTruth {
    float self_awareness;      // Understanding of personal psyche
    float collective_connection; // Link to shared consciousness
    bool reality_shaper;       // Ability to influence reality
    Dictionary manifestations;  // Specific truth expressions
};

// Core personality traits with psychological elements
struct CoreTraits {
    std::vector<String> fears;
    std::vector<String> desires;
    std::vector<String> defense_mechanisms;
    std::vector<ShadowAspect> repressed_qualities;
    std::vector<SurrealState> symbolic_states;
    
    // New psychological systems
    float resonance_intensity;    // Strength of psychological resonance
    float reality_distortion;     // Degree of perception shift
    std::vector<String> inner_revelations; // Personal truths discovered
    
    // State tracking
    ResonanceState resonance_state;
    PerceptionLayer perception_layer;
    InnerTruth inner_truth;
    
    // Integration metrics
    float consciousness_integration; // How well different states are integrated
    float reality_integration;      // How well different layers are processed
    std::vector<String> psychological_echoes; // Traces of significant experiences
};

// Personality development levels with shadow integration
struct DevelopmentLevel {
    int level;  // 1-9 (healthy to unhealthy)
    String description;
    std::vector<BehaviorPattern> characteristic_behaviors;
    float shadow_integration_level;
    std::vector<SurrealState> psychological_manifestations;
};

// Main personality profile
class PersonalityProfile : public Node {
    GDCLASS(PersonalityProfile, Node)

private:
    int primary_type = 0;
    float development_level = 0.0f;
    float blood_corruption_level = 0.0f;
    float cosmic_awareness = 0.0f;
    float transformation_threshold = 0.8f;
    float consciousness_threshold = 0.7f;
    float consciousness_integration = 0.0f;
    float reality_integration = 0.0f;
    std::vector<String> psychological_echoes;
    std::map<String, float> trait_values;
    std::map<String, float> dsm_dimensions;
    std::map<String, float> emotional_state;
    
    float stress_level = 0.0f;
    std::map<String, float> behavioral_history;

protected:
    static void _bind_methods();

public:
    PersonalityProfile();
    ~PersonalityProfile();

    void set_enneagram_type(int type);
    int get_enneagram_type() const;
    void set_development_level(float level);
    float get_development_level() const;
    void update_development_level(float delta);
    void process_blood_resonance(float intensity);
    void handle_cosmic_insight(float intensity);
    void process_event(const Dictionary& event);
    void update_beast_state(float delta);
    Vector2 get_position() const;
    Dictionary get_emotional_state() const;
    float get_trait(const String& trait_name) const;
    bool get_beast_state() const;
    float get_insight_level() const;
    Array get_blood_memories() const;
    bool has_eyes_on_the_inside() const;
    Dictionary get_cosmic_state() const;
    Dictionary to_dictionary() const;
    void from_dictionary(const Dictionary& dict);

    void add_shadow_aspect(const String& aspect_name);
    void add_surreal_state(const String& state_name);
    void add_behavior_pattern(const String& pattern_name);
    void process_stress_response();
    void process_projection();
    void handle_stress(const Dictionary& stressor);
    void update_emotional_state(const String& emotion, float value);
    void set_dsm_dimension(const String& dimension, float value);
    float get_dsm_dimension(const String& dimension) const;
    Dictionary get_current_state() const;
    Vector2 get_global_position() const;
};

} 