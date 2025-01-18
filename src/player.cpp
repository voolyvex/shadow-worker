#include "player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/node.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Player::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("handle_interaction"), &Player::handle_interaction);
    
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
}

Player::Player() {
    speed = 200.0f;  // Default speed as per specification
    acceleration = 2000.0f;
    friction = 1000.0f;
    is_moving = false;
    can_interact = true;
    current_interactable = nullptr;
}

Player::~Player() {
    // Cleanup if needed
}

void Player::_init() {
    // Initialize any additional properties
}

void Player::_ready() {
    // Called when the node enters the scene tree
    set_physics_process(true);
    set_process(true);
}

void Player::_process(double delta) {
    // Handle non-physics related updates
    handle_interaction();
}

void Player::_physics_process(double delta) {
    handle_input();
    apply_movement(delta);
}

void Player::handle_input() {
    input_vector = Vector2(0, 0);
    
    Input* input = Input::get_singleton();
    if (input->is_action_pressed("move_right")) input_vector.x += 1;
    if (input->is_action_pressed("move_left")) input_vector.x -= 1;
    if (input->is_action_pressed("move_down")) input_vector.y += 1;
    if (input->is_action_pressed("move_up")) input_vector.y -= 1;
    
    input_vector = input_vector.normalized();
}

void Player::apply_movement(double delta) {
    Vector2 current_velocity = get_velocity();
    
    if (input_vector != Vector2(0, 0)) {
        current_velocity = current_velocity.move_toward(input_vector * speed, acceleration * delta);
        is_moving = true;
    } else {
        current_velocity = current_velocity.move_toward(Vector2(0, 0), friction * delta);
        is_moving = false;
    }
    
    set_velocity(current_velocity);
    move_and_slide();
    handle_collision();
}

void Player::handle_collision() {
    // Handle any specific collision responses
    for (int i = 0; i < get_slide_collision_count(); i++) {
        Ref<KinematicCollision2D> collision = get_slide_collision(i);
        if (collision.is_valid()) {
            // Handle collision response if needed
            Object* collider_obj = collision->get_collider();
            Node* collider = Object::cast_to<Node>(collider_obj);
            if (collider && collider->is_in_group("interactable")) {
                set_interactable(collider);
            }
        }
    }
}

void Player::handle_interaction() {
    if (!can_interact || !current_interactable) return;
    
    Input* input = Input::get_singleton();
    if (input->is_action_just_pressed("interact")) {
        // Call interaction method on the interactable object
    }
}

void Player::set_interactable(Node* interactable) {
    current_interactable = interactable;
}

Node* Player::get_interactable() const {
    return current_interactable;
}

void Player::set_speed(float p_speed) {
    speed = p_speed;
}

float Player::get_speed() const {
    return speed;
} 