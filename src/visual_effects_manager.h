#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {

class RealitySystem;

class VisualEffectsManager : public Node {
    GDCLASS(VisualEffectsManager, Node)

private:
    RealitySystem* reality_system;
    Array active_effects;
    Dictionary effect_properties;

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _process(double delta) override;
    
    void set_reality_system(RealitySystem* system);
    void create_visual_effect(const Vector2& position, const String& effect_type, float intensity);
    void update_effect(int effect_id, const Dictionary& properties);
    void remove_effect(int effect_id);
    
    VisualEffectsManager();
    ~VisualEffectsManager();
};

} 