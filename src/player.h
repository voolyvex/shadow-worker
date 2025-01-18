#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/input.hpp>
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
    bool is_moving;

    // Interaction state
    bool can_interact;
    Node* current_interactable;

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
    
    // Getters and setters
    void set_speed(float p_speed);
    float get_speed() const;
};

}

#endif // PLAYER_H 