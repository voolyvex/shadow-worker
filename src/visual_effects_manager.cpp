#include "visual_effects_manager.h"
#include "reality_system.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

void VisualEffectsManager::_bind_methods() {
    // Core functionality
    ClassDB::bind_method(D_METHOD("_ready"), &VisualEffectsManager::_ready);
    ClassDB::bind_method(D_METHOD("_process", "delta"), &VisualEffectsManager::_process);
    
    // Effect management
    ClassDB::bind_method(D_METHOD("start_effect", "effect_name", "intensity", "duration"), 
        &VisualEffectsManager::start_effect);
    ClassDB::bind_method(D_METHOD("stop_effect", "effect_name"), 
        &VisualEffectsManager::stop_effect);
    ClassDB::bind_method(D_METHOD("update_effect_properties", "effect_name", "properties"), 
        &VisualEffectsManager::update_effect_properties);
    
    // Reality system integration
    ClassDB::bind_method(D_METHOD("set_reality_system", "system"), 
        &VisualEffectsManager::set_reality_system);
    ClassDB::bind_method(D_METHOD("handle_reality_anomaly", "anomaly_data"), 
        &VisualEffectsManager::handle_reality_anomaly);
    ClassDB::bind_method(D_METHOD("handle_consciousness_field", "field_data"), 
        &VisualEffectsManager::handle_consciousness_field);
    
    // Global effects
    ClassDB::bind_method(D_METHOD("set_global_intensity", "intensity"), 
        &VisualEffectsManager::set_global_intensity);
    ClassDB::bind_method(D_METHOD("get_global_intensity"), 
        &VisualEffectsManager::get_global_intensity);
    
    // State queries
    ClassDB::bind_method(D_METHOD("is_effect_active", "effect_name"), 
        &VisualEffectsManager::is_effect_active);
    ClassDB::bind_method(D_METHOD("get_effect_parameters", "effect_name"), 
        &VisualEffectsManager::get_effect_parameters);
    ClassDB::bind_method(D_METHOD("get_active_effects"), 
        &VisualEffectsManager::get_active_effects);
}

VisualEffectsManager::VisualEffectsManager() : 
    reality_system(nullptr),
    global_intensity(0.0f) {
}

VisualEffectsManager::~VisualEffectsManager() {
    // Cleanup
    active_effects.clear();
}

void VisualEffectsManager::_ready() {
    // Load shader
    Ref<Resource> shader_resource = ResourceLoader::get_singleton()->load("res://shaders/reality_distortion.gdshader");
    distortion_material.instantiate();
    distortion_material->set_shader(shader_resource);
    
    // Get viewport
    game_viewport = get_viewport();
    if (game_viewport) {
        game_viewport->set_transparent_background(true);
    }
}

void VisualEffectsManager::_process(double delta) {
    process_effect_transitions(delta);
    update_shader_parameters();
    cleanup_expired_effects();
}

void VisualEffectsManager::start_effect(const String& effect_name, float intensity, float duration) {
    EffectParameters params;
    params.intensity = Math::clamp(intensity, 0.0f, 1.0f);
    params.duration = duration;
    params.elapsed_time = 0.0f;
    params.is_active = true;
    
    // Set default properties based on effect type
    Dictionary props;
    if (effect_name == "ripple") {
        props["speed"] = 2.0f;
        props["frequency"] = 10.0f;
    } else if (effect_name == "fragment") {
        props["size"] = 0.2f;
        props["rotation_speed"] = 1.0f;
    } else if (effect_name == "merge") {
        props["blend_factor"] = 0.0f;
    } else if (effect_name == "dissolve") {
        props["noise_scale"] = 30.0f;
    } else if (effect_name == "crystallize") {
        props["refraction"] = 1.2f;
    }
    params.properties = props;
    
    active_effects[effect_name] = params;
    
    // Update global intensity
    global_intensity = Math::max(global_intensity, intensity * 0.5f);
}

void VisualEffectsManager::stop_effect(const String& effect_name) {
    auto it = active_effects.find(effect_name);
    if (it != active_effects.end()) {
        it->second.is_active = false;
    }
}

void VisualEffectsManager::update_effect_properties(const String& effect_name, const Dictionary& properties) {
    auto it = active_effects.find(effect_name);
    if (it != active_effects.end()) {
        // Merge new properties with existing ones
        Dictionary current_props = it->second.properties;
        Array keys = properties.keys();
        for (int i = 0; i < keys.size(); i++) {
            String key = keys[i];
            current_props[key] = properties[key];
        }
        it->second.properties = current_props;
    }
}

void VisualEffectsManager::set_reality_system(RealitySystem* system) {
    reality_system = system;
}

void VisualEffectsManager::handle_reality_anomaly(const Dictionary& anomaly_data) {
    if (!anomaly_data.has("effect") || !anomaly_data.has("intensity")) return;
    
    DistortionEffect effect = static_cast<DistortionEffect>(static_cast<int>(anomaly_data["effect"]));
    float intensity = anomaly_data["intensity"];
    
    // Convert reality effect to visual effect
    String effect_name;
    switch (effect) {
        case DistortionEffect::RIPPLE:
            effect_name = "ripple";
            break;
        case DistortionEffect::FRAGMENT:
            effect_name = "fragment";
            break;
        case DistortionEffect::MERGE:
            effect_name = "merge";
            break;
        case DistortionEffect::DISSOLVE:
            effect_name = "dissolve";
            break;
        case DistortionEffect::CRYSTALLIZE:
            effect_name = "crystallize";
            break;
        default:
            return;
    }
    
    // Start the visual effect
    start_effect(effect_name, intensity, 10.0f); // Default duration
    
    // Apply any additional properties from the anomaly
    if (anomaly_data.has("properties")) {
        update_effect_properties(effect_name, anomaly_data["properties"]);
    }
}

void VisualEffectsManager::handle_consciousness_field(const Dictionary& field_data) {
    if (!field_data.has("type") || !field_data.has("strength")) return;
    
    InfluenceType type = static_cast<InfluenceType>(static_cast<int>(field_data["type"]));
    float strength = field_data["strength"];
    
    // Convert influence type to visual effect
    String effect_name;
    Dictionary properties;
    
    switch (type) {
        case InfluenceType::EMOTIONAL:
            effect_name = "ripple";
            properties["frequency"] = 5.0f;
            properties["speed"] = 1.0f;
            break;
        case InfluenceType::COGNITIVE:
            effect_name = "crystallize";
            properties["refraction"] = 1.5f;
            break;
        case InfluenceType::PERCEPTUAL:
            effect_name = "fragment";
            properties["size"] = 0.3f;
            break;
        case InfluenceType::BEHAVIORAL:
            effect_name = "merge";
            properties["blend_factor"] = 0.5f;
            break;
        case InfluenceType::COLLECTIVE:
            effect_name = "dissolve";
            properties["noise_scale"] = 20.0f;
            break;
        default:
            return;
    }
    
    // Start the visual effect
    start_effect(effect_name, strength, 5.0f); // Shorter duration for consciousness effects
    update_effect_properties(effect_name, properties);
}

void VisualEffectsManager::set_global_intensity(float intensity) {
    global_intensity = Math::clamp(intensity, 0.0f, 1.0f);
    if (distortion_material.is_valid()) {
        distortion_material->set_shader_parameter("global_distortion", global_intensity);
    }
}

float VisualEffectsManager::get_global_intensity() const {
    return global_intensity;
}

bool VisualEffectsManager::is_effect_active(const String& effect_name) const {
    auto it = active_effects.find(effect_name);
    return it != active_effects.end() && it->second.is_active;
}

Dictionary VisualEffectsManager::get_effect_parameters(const String& effect_name) const {
    Dictionary params;
    auto it = active_effects.find(effect_name);
    if (it != active_effects.end()) {
        params["intensity"] = it->second.intensity;
        params["duration"] = it->second.duration;
        params["elapsed_time"] = it->second.elapsed_time;
        params["properties"] = it->second.properties;
        params["is_active"] = it->second.is_active;
    }
    return params;
}

Array VisualEffectsManager::get_active_effects() const {
    Array effects;
    for (const auto& pair : active_effects) {
        if (pair.second.is_active) {
            Dictionary effect;
            effect["name"] = pair.first;
            effect["intensity"] = pair.second.intensity;
            effect["duration"] = pair.second.duration;
            effect["elapsed_time"] = pair.second.elapsed_time;
            effect["properties"] = pair.second.properties;
            effects.push_back(effect);
        }
    }
    return effects;
}

void VisualEffectsManager::update_shader_parameters() {
    if (!distortion_material.is_valid()) return;
    
    // Update global parameters
    distortion_material->set_shader_parameter("global_distortion", global_intensity);
    distortion_material->set_shader_parameter("time_elapsed", get_process_delta_time());
    
    // Reset all effect parameters
    distortion_material->set_shader_parameter("ripple_intensity", 0.0f);
    distortion_material->set_shader_parameter("fragment_size", 0.0f);
    distortion_material->set_shader_parameter("merge_factor", 0.0f);
    distortion_material->set_shader_parameter("dissolve_progress", 0.0f);
    distortion_material->set_shader_parameter("crystallize_intensity", 0.0f);
    
    // Update active effect parameters
    for (const auto& pair : active_effects) {
        if (!pair.second.is_active) continue;
        
        const String& effect_name = pair.first;
        const EffectParameters& params = pair.second;
        
        if (effect_name == "ripple") {
            distortion_material->set_shader_parameter("ripple_intensity", params.intensity);
            distortion_material->set_shader_parameter("ripple_speed", params.properties["speed"]);
            distortion_material->set_shader_parameter("ripple_frequency", params.properties["frequency"]);
        }
        else if (effect_name == "fragment") {
            distortion_material->set_shader_parameter("fragment_size", params.intensity);
            distortion_material->set_shader_parameter("fragment_rotation", params.properties["rotation_speed"]);
        }
        else if (effect_name == "merge") {
            distortion_material->set_shader_parameter("merge_factor", params.intensity);
        }
        else if (effect_name == "dissolve") {
            distortion_material->set_shader_parameter("dissolve_progress", params.intensity);
            distortion_material->set_shader_parameter("dissolve_noise_scale", params.properties["noise_scale"]);
        }
        else if (effect_name == "crystallize") {
            distortion_material->set_shader_parameter("crystallize_intensity", params.intensity);
            distortion_material->set_shader_parameter("refraction_strength", params.properties["refraction"]);
        }
    }
}

void VisualEffectsManager::process_effect_transitions(float delta) {
    float total_intensity = 0.0f;
    int active_count = 0;
    
    for (auto& pair : active_effects) {
        EffectParameters& params = pair.second;
        if (!params.is_active) continue;
        
        // Update elapsed time
        params.elapsed_time += delta;
        
        // Calculate fade out near the end of duration
        float fade_time = 1.0f;
        if (params.elapsed_time > params.duration - fade_time) {
            float fade_factor = (params.duration - params.elapsed_time) / fade_time;
            params.intensity *= Math::max(0.0f, fade_factor);
        }
        
        // Accumulate total intensity
        total_intensity += params.intensity;
        active_count++;
    }
    
    // Update global intensity based on active effects
    if (active_count > 0) {
        float avg_intensity = total_intensity / active_count;
        global_intensity = Math::lerp(global_intensity, avg_intensity, delta * 2.0f);
    } else {
        global_intensity = Math::lerp(global_intensity, 0.0f, delta * 2.0f);
    }
}

void VisualEffectsManager::cleanup_expired_effects() {
    for (auto it = active_effects.begin(); it != active_effects.end();) {
        if (!it->second.is_active || it->second.elapsed_time >= it->second.duration) {
            it = active_effects.erase(it);
        } else {
            ++it;
        }
    }
} 