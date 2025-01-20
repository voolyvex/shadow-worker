#ifndef VISUAL_EFFECTS_MANAGER_H
#define VISUAL_EFFECTS_MANAGER_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <map>
#include <string>

namespace godot {

class RealitySystem;

// Structure to track effect parameters
struct EffectParameters {
    float intensity;
    float duration;
    float elapsed_time;
    Dictionary properties;
    bool is_active;
};

class VisualEffectsManager : public Node2D {
    GDCLASS(VisualEffectsManager, Node2D)

private:
    // Core components
    RealitySystem* reality_system;
    Ref<ShaderMaterial> distortion_material;
    Viewport* game_viewport;
    
    // Effect tracking
    std::map<String, EffectParameters> active_effects;
    float global_intensity;
    
    // Internal methods
    void update_shader_parameters();
    void process_effect_transitions(float delta);
    void cleanup_expired_effects();
    void apply_global_distortion();

protected:
    static void _bind_methods();

public:
    VisualEffectsManager();
    ~VisualEffectsManager();

    // Core functionality
    void _ready() override;
    void _process(double delta) override;
    
    // Effect management
    void start_effect(const String& effect_name, float intensity, float duration);
    void stop_effect(const String& effect_name);
    void update_effect_properties(const String& effect_name, const Dictionary& properties);
    
    // Reality system integration
    void set_reality_system(RealitySystem* system);
    void handle_reality_anomaly(const Dictionary& anomaly_data);
    void handle_consciousness_field(const Dictionary& field_data);
    
    // Global effects
    void set_global_intensity(float intensity);
    float get_global_intensity() const;
    
    // State queries
    bool is_effect_active(const String& effect_name) const;
    Dictionary get_effect_parameters(const String& effect_name) const;
    Array get_active_effects() const;
};

}

#endif // VISUAL_EFFECTS_MANAGER_H 