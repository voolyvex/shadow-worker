#include "reality_system.h"
#include "personality_system.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void RealitySystem::_bind_methods() {
    // Core functionality
    ClassDB::bind_method(D_METHOD("_process", "delta"), &RealitySystem::_process);
    ClassDB::bind_method(D_METHOD("_physics_process", "delta"), &RealitySystem::_physics_process);
    
    // Reality manipulation
    ClassDB::bind_method(D_METHOD("create_reality_anomaly", "position", "effect", "intensity"), 
        &RealitySystem::create_reality_anomaly);
    ClassDB::bind_method(D_METHOD("remove_reality_anomaly", "position"), 
        &RealitySystem::remove_reality_anomaly);
    ClassDB::bind_method(D_METHOD("is_position_distorted", "position"), 
        &RealitySystem::is_position_distorted);
    ClassDB::bind_method(D_METHOD("get_distortion_intensity", "position"), 
        &RealitySystem::get_distortion_intensity);
    
    // Consciousness field management
    ClassDB::bind_method(D_METHOD("create_consciousness_field", "origin", "type", "strength"), 
        &RealitySystem::create_consciousness_field);
    ClassDB::bind_method(D_METHOD("update_consciousness_field", "field_id", "properties"), 
        &RealitySystem::update_consciousness_field);
    ClassDB::bind_method(D_METHOD("remove_consciousness_field", "field_id"), 
        &RealitySystem::remove_consciousness_field);
    
    // Entity influence
    ClassDB::bind_method(D_METHOD("register_entity", "entity"), &RealitySystem::register_entity);
    ClassDB::bind_method(D_METHOD("unregister_entity", "entity"), &RealitySystem::unregister_entity);
    ClassDB::bind_method(D_METHOD("apply_influence", "target", "type", "strength"), 
        &RealitySystem::apply_influence);
    
    // State queries
    ClassDB::bind_method(D_METHOD("get_reality_state"), &RealitySystem::get_reality_state);
    ClassDB::bind_method(D_METHOD("get_active_anomalies"), &RealitySystem::get_active_anomalies);
    ClassDB::bind_method(D_METHOD("get_consciousness_fields"), &RealitySystem::get_consciousness_fields);
    ClassDB::bind_method(D_METHOD("get_global_distortion"), &RealitySystem::get_global_distortion);
    
    // Configuration
    ClassDB::bind_method(D_METHOD("set_influence_threshold", "type", "threshold"), 
        &RealitySystem::set_influence_threshold);
    ClassDB::bind_method(D_METHOD("get_influence_threshold", "type"), 
        &RealitySystem::get_influence_threshold);
    ClassDB::bind_method(D_METHOD("set_global_distortion", "level"), 
        &RealitySystem::set_global_distortion);
}

RealitySystem::RealitySystem() : global_distortion_level(0.0f) {
    // Initialize influence thresholds
    influence_thresholds[InfluenceType::EMOTIONAL] = 0.3f;
    influence_thresholds[InfluenceType::COGNITIVE] = 0.4f;
    influence_thresholds[InfluenceType::PERCEPTUAL] = 0.5f;
    influence_thresholds[InfluenceType::BEHAVIORAL] = 0.6f;
    influence_thresholds[InfluenceType::COLLECTIVE] = 0.7f;
}

RealitySystem::~RealitySystem() {
    // Cleanup
    active_anomalies.clear();
    consciousness_fields.clear();
    influenced_entities.clear();
}

void RealitySystem::_process(double delta) {
    update_anomalies(delta);
    process_consciousness_fields(delta);
    calculate_reality_state();
    apply_influence_effects();
    cleanup_expired_effects();
}

void RealitySystem::_physics_process(double delta) {
    // Handle physics-based reality distortions if needed
}

void RealitySystem::create_reality_anomaly(const Vector3& position, DistortionEffect effect, float intensity) {
    RealityAnomaly anomaly;
    anomaly.position = position;
    anomaly.effect = effect;
    anomaly.intensity = Math::clamp(intensity, 0.0f, 1.0f);
    anomaly.radius = 5.0f + (intensity * 10.0f); // Scale radius with intensity
    anomaly.is_active = true;
    anomaly.duration = 10.0f; // Default duration
    anomaly.elapsed_time = 0.0f;
    
    // Set effect-specific properties
    Dictionary props;
    switch (effect) {
        case DistortionEffect::RIPPLE:
            props["wave_speed"] = 2.0f;
            props["wave_amplitude"] = intensity * 0.5f;
            break;
        case DistortionEffect::FRAGMENT:
            props["fragment_size"] = 1.0f - intensity;
            props["rotation_speed"] = intensity * 2.0f;
            break;
        case DistortionEffect::MERGE:
            props["merge_speed"] = intensity;
            props["blend_factor"] = 0.0f;
            break;
        case DistortionEffect::DISSOLVE:
            props["dissolve_rate"] = intensity * 0.5f;
            props["reform_rate"] = intensity * 0.3f;
            break;
        case DistortionEffect::CRYSTALLIZE:
            props["clarity"] = intensity;
            props["refraction"] = 1.0f + intensity;
            break;
        default:
            break;
    }
    anomaly.properties = props;
    
    active_anomalies.push_back(anomaly);
    
    // Update global distortion
    global_distortion_level = Math::min(1.0f, global_distortion_level + (intensity * 0.2f));
}

void RealitySystem::remove_reality_anomaly(const Vector3& position) {
    for (auto it = active_anomalies.begin(); it != active_anomalies.end(); ++it) {
        if (it->position.distance_to(position) < 0.1f) {
            global_distortion_level = Math::max(0.0f, global_distortion_level - (it->intensity * 0.2f));
            active_anomalies.erase(it);
            break;
        }
    }
}

bool RealitySystem::is_position_distorted(const Vector3& position) const {
    for (const auto& anomaly : active_anomalies) {
        if (anomaly.position.distance_to(position) <= anomaly.radius) {
            return true;
        }
    }
    return false;
}

float RealitySystem::get_distortion_intensity(const Vector3& position) const {
    float max_intensity = 0.0f;
    for (const auto& anomaly : active_anomalies) {
        float distance = anomaly.position.distance_to(position);
        if (distance <= anomaly.radius) {
            float intensity = anomaly.intensity * (1.0f - (distance / anomaly.radius));
            max_intensity = Math::max(max_intensity, intensity);
        }
    }
    return max_intensity;
}

void RealitySystem::create_consciousness_field(const Vector3& origin, InfluenceType type, float strength) {
    ConsciousnessField field;
    field.origin = origin;
    field.influence_type = type;
    field.field_strength = Math::clamp(strength, 0.0f, 1.0f);
    field.radius = 10.0f + (strength * 20.0f); // Scale radius with strength
    
    // Set influence properties based on type
    Dictionary props;
    switch (type) {
        case InfluenceType::EMOTIONAL:
            props["empathy_factor"] = strength;
            props["emotional_resonance"] = true;
            break;
        case InfluenceType::COGNITIVE:
            props["thought_sync"] = strength * 0.7f;
            props["clarity_boost"] = strength * 0.5f;
            break;
        case InfluenceType::PERCEPTUAL:
            props["reality_blend"] = strength;
            props["perception_shift"] = true;
            break;
        case InfluenceType::BEHAVIORAL:
            props["behavior_sync"] = strength * 0.8f;
            props["pattern_influence"] = strength * 0.6f;
            break;
        case InfluenceType::COLLECTIVE:
            props["group_resonance"] = strength;
            props["consciousness_merge"] = strength > 0.7f;
            break;
    }
    field.influence_properties = props;
    
    consciousness_fields.push_back(field);
}

void RealitySystem::update_consciousness_field(int field_id, const Dictionary& properties) {
    if (field_id >= 0 && field_id < (int)consciousness_fields.size()) {
        consciousness_fields[field_id].influence_properties = properties;
    }
}

void RealitySystem::remove_consciousness_field(int field_id) {
    if (field_id >= 0 && field_id < (int)consciousness_fields.size()) {
        consciousness_fields.erase(consciousness_fields.begin() + field_id);
    }
}

void RealitySystem::register_entity(PersonalityProfile* entity) {
    if (entity && std::find(influenced_entities.begin(), influenced_entities.end(), entity) == influenced_entities.end()) {
        influenced_entities.push_back(entity);
    }
}

void RealitySystem::unregister_entity(PersonalityProfile* entity) {
    auto it = std::find(influenced_entities.begin(), influenced_entities.end(), entity);
    if (it != influenced_entities.end()) {
        influenced_entities.erase(it);
    }
}

void RealitySystem::apply_influence(PersonalityProfile* target, InfluenceType type, float strength) {
    if (!target) return;
    
    float threshold = influence_thresholds[type];
    if (strength < threshold) return;
    
    Dictionary influence_event;
    influence_event["type"] = static_cast<int>(type);
    influence_event["strength"] = strength;
    influence_event["source"] = "reality_system";
    
    // Add type-specific effects
    switch (type) {
        case InfluenceType::EMOTIONAL:
            influence_event["emotional_impact"] = strength * 0.8f;
            influence_event["resonance_level"] = strength * 0.6f;
            break;
        case InfluenceType::COGNITIVE:
            influence_event["thought_clarity"] = strength * 0.7f;
            influence_event["insight_boost"] = strength > 0.8f;
            break;
        case InfluenceType::PERCEPTUAL:
            influence_event["reality_shift"] = strength * 0.9f;
            influence_event["perception_expand"] = strength > 0.6f;
            break;
        case InfluenceType::BEHAVIORAL:
            influence_event["pattern_change"] = strength * 0.5f;
            influence_event["adaptation_rate"] = strength * 0.4f;
            break;
        case InfluenceType::COLLECTIVE:
            influence_event["group_sync"] = strength * 0.8f;
            influence_event["consciousness_blend"] = strength > 0.7f;
            break;
    }
    
    target->process_event(influence_event);
}

void RealitySystem::update_anomalies(float delta) {
    for (auto it = active_anomalies.begin(); it != active_anomalies.end();) {
        it->elapsed_time += delta;
        
        // Update effect properties
        switch (it->effect) {
            case DistortionEffect::RIPPLE:
                it->properties["wave_phase"] = Math::fmod(it->elapsed_time * it->properties["wave_speed"], Math_TAU);
                break;
            case DistortionEffect::FRAGMENT:
                it->properties["rotation"] = Math::fmod(it->elapsed_time * it->properties["rotation_speed"], Math_TAU);
                break;
            case DistortionEffect::MERGE:
                it->properties["blend_factor"] = Math::min(1.0f, it->properties["blend_factor"] + 
                    (delta * it->properties["merge_speed"]));
                break;
            case DistortionEffect::DISSOLVE:
                it->properties["dissolve_progress"] = Math::min(1.0f, it->elapsed_time * it->properties["dissolve_rate"]);
                break;
            case DistortionEffect::CRYSTALLIZE:
                it->properties["intensity"] = Math::min(1.0f, it->elapsed_time * it->properties["clarity"]);
                break;
            default:
                break;
        }
        
        // Remove expired anomalies
        if (it->elapsed_time >= it->duration) {
            global_distortion_level = Math::max(0.0f, global_distortion_level - (it->intensity * 0.2f));
            it = active_anomalies.erase(it);
        } else {
            ++it;
        }
    }
}

void RealitySystem::process_consciousness_fields(float delta) {
    for (auto& field : consciousness_fields) {
        // Update field properties based on type
        switch (field.influence_type) {
            case InfluenceType::EMOTIONAL:
                field.influence_properties["resonance_phase"] = Math::fmod(
                    field.influence_properties["resonance_phase"] + delta, Math_TAU);
                break;
            case InfluenceType::COLLECTIVE:
                field.influence_properties["sync_level"] = Math::min(1.0f,
                    field.influence_properties["sync_level"] + (delta * 0.1f));
                break;
            default:
                break;
        }
        
        // Apply field effects to entities
        for (auto* entity : influenced_entities) {
            Vector3 entity_pos = entity->get_position();
            float distance = field.origin.distance_to(entity_pos);
            
            if (distance <= field.radius) {
                float influence = field.field_strength * (1.0f - (distance / field.radius));
                apply_influence(entity, field.influence_type, influence);
            }
        }
    }
}

void RealitySystem::calculate_reality_state() {
    Dictionary state;
    state["global_distortion"] = global_distortion_level;
    
    // Calculate average anomaly intensity
    float avg_intensity = 0.0f;
    if (!active_anomalies.empty()) {
        for (const auto& anomaly : active_anomalies) {
            avg_intensity += anomaly.intensity;
        }
        avg_intensity /= active_anomalies.size();
    }
    state["average_intensity"] = avg_intensity;
    
    // Track active effects
    Array active_effects;
    for (const auto& anomaly : active_anomalies) {
        Dictionary effect;
        effect["type"] = static_cast<int>(anomaly.effect);
        effect["position"] = anomaly.position;
        effect["intensity"] = anomaly.intensity;
        effect["radius"] = anomaly.radius;
        effect["properties"] = anomaly.properties;
        active_effects.push_back(effect);
    }
    state["active_effects"] = active_effects;
    
    // Track consciousness field influence
    Dictionary field_influence;
    for (const auto& field : consciousness_fields) {
        String type_key = String::num(static_cast<int>(field.influence_type));
        field_influence[type_key] = field.field_strength;
    }
    state["field_influence"] = field_influence;
    
    reality_state = state;
}

void RealitySystem::apply_influence_effects() {
    for (auto* entity : influenced_entities) {
        Vector3 entity_pos = entity->get_position();
        
        // Calculate combined influence from all active effects
        float total_influence = 0.0f;
        InfluenceType dominant_type = InfluenceType::EMOTIONAL;
        float max_influence = 0.0f;
        
        for (const auto& field : consciousness_fields) {
            float distance = field.origin.distance_to(entity_pos);
            if (distance <= field.radius) {
                float influence = field.field_strength * (1.0f - (distance / field.radius));
                total_influence += influence;
                
                if (influence > max_influence) {
                    max_influence = influence;
                    dominant_type = field.influence_type;
                }
            }
        }
        
        // Apply dominant influence if above threshold
        if (max_influence > influence_thresholds[dominant_type]) {
            apply_influence(entity, dominant_type, max_influence);
        }
    }
}

void RealitySystem::cleanup_expired_effects() {
    // Remove expired anomalies
    active_anomalies.erase(
        std::remove_if(active_anomalies.begin(), active_anomalies.end(),
            [](const RealityAnomaly& a) { return !a.is_active || a.elapsed_time >= a.duration; }),
        active_anomalies.end()
    );
    
    // Remove empty consciousness fields
    consciousness_fields.erase(
        std::remove_if(consciousness_fields.begin(), consciousness_fields.end(),
            [](const ConsciousnessField& f) { return f.affected_entities.empty(); }),
        consciousness_fields.end()
    );
}

Dictionary RealitySystem::get_reality_state() const {
    return reality_state;
}

Array RealitySystem::get_active_anomalies() const {
    Array anomalies;
    for (const auto& anomaly : active_anomalies) {
        Dictionary a;
        a["position"] = anomaly.position;
        a["effect"] = static_cast<int>(anomaly.effect);
        a["intensity"] = anomaly.intensity;
        a["radius"] = anomaly.radius;
        a["properties"] = anomaly.properties;
        anomalies.push_back(a);
    }
    return anomalies;
}

Array RealitySystem::get_consciousness_fields() const {
    Array fields;
    for (const auto& field : consciousness_fields) {
        Dictionary f;
        f["origin"] = field.origin;
        f["type"] = static_cast<int>(field.influence_type);
        f["strength"] = field.field_strength;
        f["radius"] = field.radius;
        f["properties"] = field.influence_properties;
        fields.push_back(f);
    }
    return fields;
}

float RealitySystem::get_global_distortion() const {
    return global_distortion_level;
}

void RealitySystem::set_influence_threshold(InfluenceType type, float threshold) {
    influence_thresholds[type] = Math::clamp(threshold, 0.0f, 1.0f);
}

float RealitySystem::get_influence_threshold(InfluenceType type) const {
    auto it = influence_thresholds.find(type);
    return it != influence_thresholds.end() ? it->second : 0.5f;
}

void RealitySystem::set_global_distortion(float level) {
    global_distortion_level = Math::clamp(level, 0.0f, 1.0f);
} 