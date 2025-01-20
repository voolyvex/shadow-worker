#ifndef PERSONALITY_SYSTEM_H
#define PERSONALITY_SYSTEM_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
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
class PersonalityProfile {
    GDCLASS(PersonalityProfile, Object)

private:
    EnneagramType primary_type;
    std::vector<EnneagramType> wings;
    DevelopmentLevel development;
    DSMDimensions dsm_traits;
    CoreTraits core_traits;
    
    // Jungian elements
    std::map<JungianArchetype, float> archetype_influence;
    std::vector<ShadowAspect> active_projections;
    float individuation_progress;
    
    // Surreal psychological states
    std::vector<SurrealState> current_symbolic_state;
    float reality_distortion_level;
    
    // Dynamic state tracking
    float stress_level;
    float growth_potential;
    std::map<String, float> emotional_state;
    Dictionary behavioral_history;
    
    // Internal beast/sovereign processing
    void update_blood_corruption(double delta);
    void process_cosmic_revelation();
    void integrate_blood_memory(const String& memory);
    void calculate_transformation_threshold();
    
    // New member variables
    float blood_corruption_level;
    float cosmic_awareness;
    float transformation_threshold;
    std::vector<Dictionary> cosmic_revelations;
    
    void update_development_level();
    void process_stress_response();
    void calculate_growth_direction();
    void integrate_shadow_aspects();
    void update_symbolic_state();
    void process_archetype_dynamics();

protected:
    static void _bind_methods();

public:
    PersonalityProfile();
    ~PersonalityProfile();
    
    // Core personality methods
    void set_enneagram_type(EnneagramType type);
    EnneagramType get_enneagram_type() const;
    void add_wing(EnneagramType wing);
    void set_development_level(int level);
    
    // DSM trait methods
    void set_dsm_dimension(const String& dimension, float value);
    float get_dsm_dimension(const String& dimension) const;
    
    // Shadow and archetype methods
    void add_shadow_aspect(const String& trait, float intensity, bool is_positive);
    void process_projection(const String& trait, Node* target);
    float get_archetype_influence(JungianArchetype archetype) const;
    Array get_active_projections() const;
    
    // Surreal state methods
    void add_surreal_state(const String& symbol, float distortion);
    Dictionary get_symbolic_state() const;
    float get_reality_distortion() const;
    
    // Behavioral methods
    void add_behavior_pattern(const String& behavior, float weight, const Dictionary& context);
    Array get_behavior_patterns() const;
    float calculate_behavior_probability(const String& behavior, const Dictionary& context) const;
    
    // State management
    void update_emotional_state(const String& emotion, float intensity);
    float get_emotional_intensity(const String& emotion) const;
    void process_event(const Dictionary& event);
    Dictionary get_current_state() const;
    
    // Development and growth
    void simulate_growth(double delta);
    void handle_stress(const Dictionary& stressor);
    Dictionary get_growth_opportunities() const;
    float get_individuation_progress() const;
    
    // Psychological systems
    void process_resonance(float intensity);
    void update_perception_layer(double delta);
    void handle_inner_revelation(const Dictionary& revelation);
    float calculate_consciousness_threshold() const;
    
    void trigger_reality_shift(float catalyst_strength);
    void resist_dissolution(float willpower);
    
    // State queries
    ResonanceState get_resonance_state() const;
    float get_consciousness_level() const;
    Array get_psychological_echoes() const;
    bool has_reality_shaping() const;
    Dictionary get_inner_state() const;
    
    // Serialization
    Dictionary to_dictionary() const;
    void from_dictionary(const Dictionary& data);
};

}

#endif // PERSONALITY_SYSTEM_H 