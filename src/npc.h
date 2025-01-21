#pragma once

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include "personality_system.h"
#include <queue>

using namespace godot;

enum class NPCState {
    IDLE = 0,
    PATROL,
    FOLLOW,
    FLEE,
    INTERACT,
    WORK
};

class NPC : public CharacterBody2D {
    GDCLASS(NPC, CharacterBody2D)

private:
    NPCState current_state = NPCState::IDLE;
    Sprite2D* sprite = nullptr;
    Vector2 velocity;
    Vector2 home_position;
    Vector2 target_position;
    float base_speed = 100.0f;
    float speed = base_speed;
    float base_patrol_radius = 100.0f;
    float patrol_radius = base_patrol_radius;
    float base_interaction_radius = 50.0f;
    float interaction_radius = base_interaction_radius;
    float base_interaction_cooldown = 2.0f;
    float interaction_cooldown = base_interaction_cooldown;
    Array dialogue_queue;
    bool is_interactable = true;
    Label* dialogue_display = nullptr;
    Area2D* detection_area = nullptr;
    float cooldown_timer = 0.0f;
    float state_timer = 0.0f;
    float decision_cooldown = 1.0f;
    String current_dialogue_state;
    PersonalityProfile* personality = nullptr;

protected:
    static void _bind_methods();

public:
    NPC();
    ~NPC();

    void _ready() override;
    void _process(double delta) override;
    void set_state(NPCState new_state);
    NPCState get_state() const;
    void set_patrol_radius(float radius);
    float get_patrol_radius() const;
    void set_interaction_radius(float radius);
    float get_interaction_radius() const;
    void set_speed(float new_speed);
    float get_speed() const;
    void set_dialogue_data(const Array& dialogue);
    void display_dialogue();
    void advance_dialogue();
    void clear_dialogue();
    bool is_interaction_ready() const;
    void interact();
    void initialize_personality();
    void update_state(double delta);
    void handle_state_idle(double delta);
    void handle_state_patrol(double delta);
    void handle_state_follow(double delta);
    void handle_state_flee(double delta);
    void handle_state_interact(double delta);
    void handle_state_work(double delta);
    void update_animation();
    void handle_movement(double delta);
    void process_personality_influence(double delta);
    Vector2 get_random_patrol_point() const;
    Vector2 calculate_avoidance_vector() const;
    float get_distance_to_target(const Vector2& target) const;
    bool is_near_workstation() const;
    bool is_work_hours() const;
    Vector2 get_nearest_workstation() const;
    bool get_is_interactable() const;
    void set_personality_trait(const String& trait, float value);
    float get_personality_trait(const String& trait) const;
    Dictionary get_personality_state() const;
    void handle_emotional_event(const String& event_type, float intensity);
    void handle_event(const Dictionary& event);

private:
    void setup_detection_area();
    void setup_dialogue_display();
    void on_player_entered(Node* body);
    void on_player_exited(Node* body);
}; 