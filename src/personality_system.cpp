#include "personality_system.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

void PersonalityProfile::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_enneagram_type", "type"), &PersonalityProfile::set_enneagram_type);
    ClassDB::bind_method(D_METHOD("get_enneagram_type"), &PersonalityProfile::get_enneagram_type);
    ClassDB::bind_method(D_METHOD("set_development_level", "level"), &PersonalityProfile::set_development_level);
    ClassDB::bind_method(D_METHOD("get_development_level"), &PersonalityProfile::get_development_level);
    ClassDB::bind_method(D_METHOD("process_blood_resonance", "intensity"), &PersonalityProfile::process_blood_resonance);
    ClassDB::bind_method(D_METHOD("handle_cosmic_insight", "intensity"), &PersonalityProfile::handle_cosmic_insight);
    ClassDB::bind_method(D_METHOD("update_beast_state", "delta"), &PersonalityProfile::update_beast_state);
    ClassDB::bind_method(D_METHOD("get_position"), &PersonalityProfile::get_position);
    ClassDB::bind_method(D_METHOD("get_emotional_state"), &PersonalityProfile::get_emotional_state);
    ClassDB::bind_method(D_METHOD("get_trait", "trait_name"), &PersonalityProfile::get_trait);
    ClassDB::bind_method(D_METHOD("get_beast_state"), &PersonalityProfile::get_beast_state);
    ClassDB::bind_method(D_METHOD("get_insight_level"), &PersonalityProfile::get_insight_level);
    ClassDB::bind_method(D_METHOD("get_blood_memories"), &PersonalityProfile::get_blood_memories);
    ClassDB::bind_method(D_METHOD("has_eyes_on_the_inside"), &PersonalityProfile::has_eyes_on_the_inside);
    ClassDB::bind_method(D_METHOD("get_cosmic_state"), &PersonalityProfile::get_cosmic_state);
    ClassDB::bind_method(D_METHOD("process_event", "event"), &PersonalityProfile::process_event);
    ClassDB::bind_method(D_METHOD("to_dictionary"), &PersonalityProfile::to_dictionary);
    ClassDB::bind_method(D_METHOD("from_dictionary", "dict"), &PersonalityProfile::from_dictionary);
    ClassDB::bind_method(D_METHOD("handle_stress", "stressor"), &PersonalityProfile::handle_stress);
    ClassDB::bind_method(D_METHOD("update_emotional_state", "emotion", "value"), &PersonalityProfile::update_emotional_state);
    ClassDB::bind_method(D_METHOD("set_dsm_dimension", "dimension", "value"), &PersonalityProfile::set_dsm_dimension);
    ClassDB::bind_method(D_METHOD("get_dsm_dimension", "dimension"), &PersonalityProfile::get_dsm_dimension);
    ClassDB::bind_method(D_METHOD("get_current_state"), &PersonalityProfile::get_current_state);
}

PersonalityProfile::PersonalityProfile() {
    primary_type = 0;
    blood_corruption_level = 0.0f;
    cosmic_awareness = 0.0f;
    transformation_threshold = 0.8f;
    consciousness_threshold = 0.7f;
    development_level = 0.0f;
    stress_level = 0.0f;
}

PersonalityProfile::~PersonalityProfile() {
    // Cleanup if needed
}

void PersonalityProfile::set_enneagram_type(int type) {
    primary_type = type;
}

int PersonalityProfile::get_enneagram_type() const {
    return primary_type;
}

void PersonalityProfile::set_development_level(float level) {
    development_level = Math::clamp(level, 0.0f, 1.0f);
}

float PersonalityProfile::get_development_level() const {
    return development_level;
}

void PersonalityProfile::update_development_level(float delta) {
    development_level = Math::clamp(development_level + delta, 0.0f, 1.0f);
}

void PersonalityProfile::process_blood_resonance(float intensity) {
    blood_corruption_level = Math::clamp(blood_corruption_level + intensity * 0.1f, 0.0f, 1.0f);
    
    // Update trait values
    trait_values["blood_corruption"] = blood_corruption_level;
    trait_values["primal_instinct"] = Math::clamp(trait_values["primal_instinct"] + intensity * 0.05f, 0.0f, 1.0f);
    trait_values["beast_affinity"] = Math::clamp(trait_values["beast_affinity"] + intensity * 0.08f, 0.0f, 1.0f);
    
    // Update DSM dimensions
    dsm_dimensions["psychoticism"] = Math::clamp(dsm_dimensions["psychoticism"] + intensity * 0.15f, 0.0f, 1.0f);
    dsm_dimensions["disinhibition"] = Math::clamp(dsm_dimensions["disinhibition"] + intensity * 0.12f, 0.0f, 1.0f);
}

void PersonalityProfile::handle_cosmic_insight(float intensity) {
    cosmic_awareness = Math::clamp(cosmic_awareness + intensity * 0.1f, 0.0f, 1.0f);
    
    // Update trait values
    trait_values["cosmic_awareness"] = cosmic_awareness;
    trait_values["eldritch_knowledge"] = Math::clamp(trait_values["eldritch_knowledge"] + intensity * 0.08f, 0.0f, 1.0f);
    trait_values["reality_perception"] = Math::clamp(trait_values["reality_perception"] + intensity * 0.06f, 0.0f, 1.0f);
    
    // Update DSM dimensions
    dsm_dimensions["psychoticism"] = Math::clamp(dsm_dimensions["psychoticism"] + intensity * 0.1f, 0.0f, 1.0f);
    dsm_dimensions["detachment"] = Math::clamp(dsm_dimensions["detachment"] + intensity * 0.15f, 0.0f, 1.0f);
}

void PersonalityProfile::update_beast_state(float delta) {
    if (blood_corruption_level >= transformation_threshold) {
        emotional_state["aggression"] = 1.0f;
        emotional_state["control"] = Math::clamp(1.0f - blood_corruption_level, 0.0f, 1.0f);
    }
}

Vector2 PersonalityProfile::get_position() const {
    return get_global_position();
}

Dictionary PersonalityProfile::get_emotional_state() const {
    Dictionary state;
    for (const auto& pair : emotional_state) {
        state[pair.first] = pair.second;
    }
    return state;
}

float PersonalityProfile::get_trait(const String& trait_name) const {
    auto it = trait_values.find(trait_name);
    return it != trait_values.end() ? it->second : 0.0f;
}

bool PersonalityProfile::get_beast_state() const {
    return blood_corruption_level >= transformation_threshold;
}

float PersonalityProfile::get_insight_level() const {
    return cosmic_awareness;
}

Array PersonalityProfile::get_blood_memories() const {
    Array memories;
    for (const auto& echo : psychological_echoes) {
        memories.push_back(echo);
    }
    return memories;
}

bool PersonalityProfile::has_eyes_on_the_inside() const {
    return cosmic_awareness >= consciousness_threshold;
}

Dictionary PersonalityProfile::get_cosmic_state() const {
    Dictionary state;
    state["cosmic_awareness"] = cosmic_awareness;
    state["consciousness_integration"] = consciousness_integration;
    state["reality_integration"] = reality_integration;
    return state;
}

void PersonalityProfile::process_event(const Dictionary& event) {
    String event_type = event["type"];
    float intensity = event["intensity"];
    
    if (event_type == "blood_resonance") {
        process_blood_resonance(intensity);
    } else if (event_type == "cosmic_insight") {
        handle_cosmic_insight(intensity);
    }
}

Dictionary PersonalityProfile::to_dictionary() const {
    Dictionary dict;
    
    // Convert trait values
    Dictionary trait_dict;
    for (const auto& pair : trait_values) {
        trait_dict[pair.first] = pair.second;
    }
    dict["trait_values"] = trait_dict;
    
    // Convert DSM dimensions
    Dictionary dsm_dict;
    for (const auto& pair : dsm_dimensions) {
        dsm_dict[pair.first] = pair.second;
    }
    dict["dsm_dimensions"] = dsm_dict;
    
    // Convert emotional state
    Dictionary emotional_dict;
    for (const auto& pair : emotional_state) {
        emotional_dict[pair.first] = pair.second;
    }
    dict["emotional_state"] = emotional_dict;
    
    return dict;
}

void PersonalityProfile::from_dictionary(const Dictionary& dict) {
    trait_values.clear();
    dsm_dimensions.clear();
    emotional_state.clear();
    
    if (dict.has("trait_values")) {
        Dictionary trait_dict = dict["trait_values"];
        Array keys = trait_dict.keys();
        for (int i = 0; i < keys.size(); i++) {
            String key = keys[i];
            trait_values[key] = static_cast<float>(static_cast<double>(trait_dict[key]));
        }
    }
    
    if (dict.has("dsm_dimensions")) {
        Dictionary dsm_dict = dict["dsm_dimensions"];
        Array keys = dsm_dict.keys();
        for (int i = 0; i < keys.size(); i++) {
            String key = keys[i];
            dsm_dimensions[key] = static_cast<float>(static_cast<double>(dsm_dict[key]));
        }
    }
    
    if (dict.has("emotional_state")) {
        Dictionary emotional_dict = dict["emotional_state"];
        Array keys = emotional_dict.keys();
        for (int i = 0; i < keys.size(); i++) {
            String key = keys[i];
            emotional_state[key] = static_cast<float>(static_cast<double>(emotional_dict[key]));
        }
    }
}

Vector2 PersonalityProfile::get_global_position() const {
    Node2D* node = Object::cast_to<Node2D>(get_parent());
    if (node) {
        return node->get_global_position();
    }
    return Vector2();
}

void PersonalityProfile::handle_stress(const Dictionary& stressor) {
    float intensity = stressor.has("intensity") ? static_cast<float>(stressor["intensity"]) : 0.5f;
    stress_level = Math::clamp(stress_level + intensity * 0.2f, 0.0f, 1.0f);
    
    // Update emotional state based on stress
    update_emotional_state("anxiety", intensity * 0.3f);
    update_emotional_state("tension", intensity * 0.4f);
}

void PersonalityProfile::update_emotional_state(const String& emotion, float value) {
    emotional_state[emotion] = Math::clamp(value, 0.0f, 1.0f);
}

void PersonalityProfile::set_dsm_dimension(const String& dimension, float value) {
    dsm_dimensions[dimension] = Math::clamp(value, 0.0f, 1.0f);
}

float PersonalityProfile::get_dsm_dimension(const String& dimension) const {
    auto it = dsm_dimensions.find(dimension);
    return it != dsm_dimensions.end() ? it->second : 0.0f;
}

Dictionary PersonalityProfile::get_current_state() const {
    Dictionary state;
    state["emotional_state"] = get_emotional_state();
    state["stress_level"] = stress_level;
    state["blood_corruption"] = blood_corruption_level;
    state["cosmic_awareness"] = cosmic_awareness;
    return state;
}
