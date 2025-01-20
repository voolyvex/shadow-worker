#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/ray_cast2d.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class Player : public CharacterBody2D {
    GDCLASS(Player, CharacterBody2D)

private:
    float speed;
    Vector2 velocity;
    Vector2 input_vector;
    
    // Movement state
    float acceleration;
    float friction;
    float max_speed;
    bool is_moving;
    bool is_sprinting;
    float sprint_multiplier;
    float stamina;
    float max_stamina;
    float stamina_regen_rate;
    float stamina_drain_rate;

    // Animation
    AnimatedSprite2D* sprite;
    String current_animation;
    
    // Interaction
    RayCast2D* interaction_ray;
    bool can_interact;
    Node* current_interactable;
    float interaction_cooldown;
    float interaction_timer;

    // Internal methods
    void update_animation();
    void update_stamina(double delta);
    void handle_sprint_input();

protected:
    static void _bind_methods();

public:
    Player();
    ~Player();

    void _init();
    void _ready();
    void _process(double delta);
    void _physics_process(double delta);
    
    // Movement methods
    void handle_input();
    void apply_movement(double delta);
    void handle_collision();
    
    // Interaction methods
    void handle_interaction();
    void set_interactable(Node* interactable);
    Node* get_interactable() const;
    bool is_interaction_ready() const;
    
    // Getters and setters
    void set_speed(float p_speed);
    float get_speed() const;
    void set_sprint_multiplier(float p_multiplier);
    float get_sprint_multiplier() const;
    float get_stamina() const;
    float get_max_stamina() const;
    void set_max_stamina(float p_max_stamina);
};

}

#endif // PLAYER_H 