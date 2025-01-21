#include "npc.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/shape2d.hpp>

using namespace godot;

void NPC::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_state", "new_state"), &NPC::set_state);
    ClassDB::bind_method(D_METHOD("get_state"), &NPC::get_state);
    ClassDB::bind_method(D_METHOD("set_patrol_radius", "radius"), &NPC::set_patrol_radius);
    ClassDB::bind_method(D_METHOD("get_patrol_radius"), &NPC::get_patrol_radius);
    ClassDB::bind_method(D_METHOD("set_interaction_radius", "radius"), &NPC::set_interaction_radius);
    ClassDB::bind_method(D_METHOD("get_interaction_radius"), &NPC::get_interaction_radius);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &NPC::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &NPC::get_speed);
    ClassDB::bind_method(D_METHOD("set_dialogue_data", "dialogue"), &NPC::set_dialogue_data);
    ClassDB::bind_method(D_METHOD("display_dialogue"), &NPC::display_dialogue);
    ClassDB::bind_method(D_METHOD("advance_dialogue"), &NPC::advance_dialogue);
    ClassDB::bind_method(D_METHOD("clear_dialogue"), &NPC::clear_dialogue);
    ClassDB::bind_method(D_METHOD("is_interaction_ready"), &NPC::is_interaction_ready);
    ClassDB::bind_method(D_METHOD("interact"), &NPC::interact);
    ClassDB::bind_method(D_METHOD("get_is_interactable"), &NPC::get_is_interactable);
    ClassDB::bind_method(D_METHOD("handle_event", "event"), &NPC::handle_event);
    
    // Register enum
    BIND_ENUM_CONSTANT(NPCState::IDLE);
    BIND_ENUM_CONSTANT(NPCState::PATROL);
    BIND_ENUM_CONSTANT(NPCState::FOLLOW);
    BIND_ENUM_CONSTANT(NPCState::FLEE);
    BIND_ENUM_CONSTANT(NPCState::INTERACT);
    BIND_ENUM_CONSTANT(NPCState::WORK);
    
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "interaction_radius"), "set_interaction_radius", "get_interaction_radius");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "patrol_radius"), "set_patrol_radius", "get_patrol_radius");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "dialogue_data"), "set_dialogue_data", "get_dialogue_data");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
    
    // Add new personality-related methods
    ClassDB::bind_method(D_METHOD("initialize_personality", "type"), &NPC::initialize_personality);
    ClassDB::bind_method(D_METHOD("set_personality_trait", "trait", "value"), &NPC::set_personality_trait);
    ClassDB::bind_method(D_METHOD("get_personality_trait", "trait"), &NPC::get_personality_trait);
    ClassDB::bind_method(D_METHOD("get_personality_state"), &NPC::get_personality_state);
    ClassDB::bind_method(D_METHOD("handle_emotional_event", "emotion", "intensity"), &NPC::handle_emotional_event);
}

NPC::NPC() :
    current_state(NPCState::IDLE),
    velocity(Vector2(0, 0)),
    base_speed(100.0f),
    speed(base_speed),
    base_patrol_radius(200.0f),
    patrol_radius(base_patrol_radius),
    base_interaction_radius(50.0f),
    interaction_radius(base_interaction_radius),
    base_interaction_cooldown(2.0f),
    interaction_cooldown(base_interaction_cooldown),
    is_interactable(true),
    sprite(nullptr),
    dialogue_display(nullptr),
    detection_area(nullptr)
{
    initialize_personality();
}

NPC::~NPC() {
}

void NPC::_ready() {
    sprite = get_node<Sprite2D>("Sprite2D");
    dialogue_display = get_node<Label>("DialogueDisplay");
    detection_area = get_node<Area2D>("DetectionArea");
    
    if (!sprite) {
        UtilityFunctions::print("Warning: Sprite2D node not found");
    }
    
    if (!dialogue_display) {
        UtilityFunctions::print("Warning: DialogueDisplay node not found");
    }
    
    if (!detection_area) {
        UtilityFunctions::print("Warning: DetectionArea node not found");
    }
    
    setup_detection_area();
    setup_dialogue_display();
    initialize_personality();
}

void NPC::_process(double delta) {
    update_state(delta);
    handle_movement(delta);
    update_animation();
    process_personality_influence(delta);
    
    // Update cooldown timer
    if (cooldown_timer > 0) {
        cooldown_timer -= delta;
    }
}

void NPC::set_state(NPCState new_state) {
    if (current_state != new_state) {
        current_state = new_state;
        state_timer = 0.0f;
    }
}

NPCState NPC::get_state() const {
    return current_state;
}

void NPC::update_state(double delta) {
    state_timer += delta;
    
    switch (current_state) {
        case NPCState::IDLE:
            handle_state_idle(delta);
            break;
        case NPCState::PATROL:
            handle_state_patrol(delta);
            break;
        case NPCState::FOLLOW:
            handle_state_follow(delta);
            break;
        case NPCState::FLEE:
            handle_state_flee(delta);
            break;
        case NPCState::INTERACT:
            handle_state_interact(delta);
            break;
        case NPCState::WORK:
            handle_state_work(delta);
            break;
    }
}

void NPC::handle_state_idle(double delta) {
    velocity = Vector2(0, 0);
    state_timer += delta;
    
    if (state_timer >= decision_cooldown) {
        state_timer = 0.0f;
        if (UtilityFunctions::randf() < 0.3f) {
            set_state(NPCState::PATROL);
        }
    }
}

void NPC::handle_state_patrol(double delta) {
    if (target_position == Vector2(0, 0)) {
        target_position = get_random_patrol_point();
    }
    
    Vector2 direction = (target_position - get_position()).normalized();
    velocity = direction * speed;
    
    if (get_position().distance_to(target_position) < 10.0f) {
        target_position = Vector2(0, 0);
        set_state(NPCState::IDLE);
    }
}

void NPC::handle_state_follow(double delta) {
    // Implementation for follow state
}

void NPC::handle_state_flee(double delta) {
    // Implementation for flee state
}

void NPC::handle_state_interact(double delta) {
    velocity = Vector2(0, 0);
    cooldown_timer += delta;
    
    if (cooldown_timer >= interaction_cooldown) {
        set_state(NPCState::IDLE);
        cooldown_timer = 0.0f;
    }
}

void NPC::handle_state_work(double delta) {
    if (!is_near_workstation()) {
        Vector2 workstation = get_nearest_workstation();
        Vector2 direction = (workstation - get_position()).normalized();
        velocity = direction * speed;
    } else {
        velocity = Vector2(0, 0);
    }
}

Vector2 NPC::get_random_patrol_point() const {
    float angle = UtilityFunctions::randf() * Math_TAU;
    float distance = UtilityFunctions::randf() * patrol_radius;
    return home_position + Vector2(Math::cos(angle), Math::sin(angle)) * distance;
}

float NPC::get_distance_to_target(const Vector2& target) const {
    return get_position().distance_to(target);
}

bool NPC::is_near_workstation() const {
    return get_distance_to_target(get_nearest_workstation()) < interaction_radius;
}

Vector2 NPC::get_nearest_workstation() const {
    // Placeholder implementation
    return Vector2(0, 0);
}

void NPC::set_dialogue_data(const Array& dialogue) {
    dialogue_queue = dialogue;
}

void NPC::display_dialogue() {
    if (!dialogue_display || dialogue_queue.is_empty()) return;
    
    Variant next_dialogue = dialogue_queue.front();
    if (next_dialogue.get_type() == Variant::STRING) {
        dialogue_display->set_text(next_dialogue);
        dialogue_display->show();
    }
}

void NPC::advance_dialogue() {
    if (!dialogue_queue.is_empty()) {
        dialogue_queue.pop_front();
        display_dialogue();
    }
}

void NPC::clear_dialogue() {
    dialogue_queue.clear();
    if (dialogue_display) {
        dialogue_display->hide();
        dialogue_display->set_text("");
    }
}

void NPC::handle_event(const Dictionary& event) {
    if (personality) {
        personality->process_event(event);
    }
}

void NPC::initialize_personality() {
    personality = memnew(PersonalityProfile);
    add_child(personality);
}

void NPC::process_personality_influence(double delta) {
    if (!personality) return;
    
    Dictionary state = personality->get_current_state();
    float stress = state["stress_level"];
    float corruption = state["blood_corruption"];
    
    // Adjust behavior based on personality state
    speed = base_speed * (1.0f + stress * 0.5f);
    patrol_radius = base_patrol_radius * (1.0f + corruption * 0.3f);
    interaction_radius = base_interaction_radius * (1.0f - stress * 0.2f);
}

void NPC::setup_detection_area() {
    if (!detection_area) return;
    
    detection_area->connect("body_entered", Callable(this, "on_player_entered"));
    detection_area->connect("body_exited", Callable(this, "on_player_exited"));
}

void NPC::setup_dialogue_display() {
    if (!dialogue_display) return;
    
    dialogue_display->hide();
    dialogue_display->set_text("");
}

void NPC::on_player_entered(Node* body) {
    if (body && body->is_in_group("player")) {
        set_state(NPCState::INTERACT);
    }
}

void NPC::on_player_exited(Node* body) {
    if (body && body->is_in_group("player")) {
        set_state(NPCState::IDLE);
        clear_dialogue();
    }
}

void NPC::handle_movement(double delta) {
    // Implement movement logic
}

void NPC::update_animation() {
    // Implement animation updates
}

bool NPC::is_interaction_ready() const {
    return cooldown_timer <= 0;
}

void NPC::interact() {
    if (!is_interaction_ready()) return;
    
    display_dialogue();
    cooldown_timer = interaction_cooldown;
}

bool NPC::get_is_interactable() const {
    return is_interactable;
}

void NPC::set_patrol_radius(float radius) {
    patrol_radius = radius;
}

float NPC::get_patrol_radius() const {
    return patrol_radius;
}

void NPC::set_interaction_radius(float radius) {
    interaction_radius = radius;
}

float NPC::get_interaction_radius() const {
    return interaction_radius;
}

void NPC::set_speed(float new_speed) {
    speed = new_speed;
}

float NPC::get_speed() const {
    return speed;
}

void NPC::handle_emotional_event(const String& emotion, float intensity) {
    if (personality) {
        Dictionary event;
        event["type"] = "emotional";
        event["emotion"] = emotion;
        event["intensity"] = intensity;
        personality->process_event(event);
    }
}

void NPC::set_personality_trait(const String& trait, float value) {
    if (personality) {
        personality->set_dsm_dimension(trait, value);
    }
}

float NPC::get_personality_trait(const String& trait) const {
    return personality ? personality->get_dsm_dimension(trait) : 0.0f;
}

Dictionary NPC::get_personality_state() const {
    return personality ? personality->get_current_state() : Dictionary();
} 