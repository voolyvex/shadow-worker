#include "player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Player::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_sprint_multiplier", "multiplier"), &Player::set_sprint_multiplier);
    ClassDB::bind_method(D_METHOD("get_sprint_multiplier"), &Player::get_sprint_multiplier);
    ClassDB::bind_method(D_METHOD("get_stamina"), &Player::get_stamina);
    ClassDB::bind_method(D_METHOD("get_max_stamina"), &Player::get_max_stamina);
    ClassDB::bind_method(D_METHOD("set_max_stamina", "max_stamina"), &Player::set_max_stamina);
    ClassDB::bind_method(D_METHOD("is_interaction_ready"), &Player::is_interaction_ready);
    
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sprint_multiplier"), "set_sprint_multiplier", "get_sprint_multiplier");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_stamina"), "set_max_stamina", "get_max_stamina");
}

Player::Player() {
    speed = 200.0f;
    max_speed = 300.0f;
    acceleration = 2000.0f;
    friction = 1000.0f;
    sprint_multiplier = 1.5f;
    
    stamina = 100.0f;
    max_stamina = 100.0f;
    stamina_regen_rate = 10.0f;
    stamina_drain_rate = 20.0f;
    
    interaction_cooldown = 0.5f;
    interaction_timer = 0.0f;
    
    is_moving = false;
    is_sprinting = false;
    can_interact = true;
    current_interactable = nullptr;
    sprite = nullptr;
    interaction_ray = nullptr;
}

Player::~Player() {
    // Cleanup if needed
}

void Player::_init() {
    // Initialize any additional properties
}

void Player::_ready() {
    sprite = Object::cast_to<AnimatedSprite2D>(get_node("AnimatedSprite2D"));
    ERR_FAIL_NULL_MSG(sprite, "Player requires an AnimatedSprite2D child node!");
    
    interaction_ray = Object::cast_to<RayCast2D>(get_node("InteractionRay"));
    ERR_FAIL_NULL_MSG(interaction_ray, "Player requires an InteractionRay (RayCast2D) child node!");
    
    set_physics_process(true);
    set_process(true);
}

void Player::_process(double delta) {
    update_animation();
    handle_interaction();
    update_stamina(delta);
    
    if (interaction_timer > 0) {
        interaction_timer -= delta;
    }
}

void Player::_physics_process(double delta) {
    handle_input();
    handle_sprint_input();
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

void Player::handle_sprint_input() {
    Input* input = Input::get_singleton();
    is_sprinting = input->is_action_pressed("sprint") && stamina > 0;
}

void Player::apply_movement(double delta) {
    float current_speed = speed * (is_sprinting ? sprint_multiplier : 1.0f);
    Vector2 target_velocity = input_vector * current_speed;
    
    if (input_vector != Vector2(0, 0)) {
        velocity = velocity.move_toward(target_velocity, acceleration * delta);
        is_moving = true;
    } else {
        velocity = velocity.move_toward(Vector2(0, 0), friction * delta);
        is_moving = false;
    }
    
    // Clamp velocity to max speed
    if (velocity.length() > max_speed) {
        velocity = velocity.normalized() * max_speed;
    }
    
    set_velocity(velocity);
    move_and_slide();
    handle_collision();
}

void Player::update_stamina(double delta) {
    if (is_sprinting && is_moving) {
        stamina = Math::max(0.0f, stamina - stamina_drain_rate * delta);
    } else if (stamina < max_stamina) {
        stamina = Math::min(max_stamina, stamina + stamina_regen_rate * delta);
    }
}

void Player::update_animation() {
    if (!sprite) return;
    
    String new_animation = "idle";
    bool flip_h = false;
    
    if (is_moving) {
        new_animation = is_sprinting ? "run" : "walk";
        
        if (input_vector.x != 0) {
            flip_h = input_vector.x < 0;
        }
    }
    
    if (new_animation != current_animation) {
        sprite->play(new_animation);
        current_animation = new_animation;
    }
    
    sprite->set_flip_h(flip_h);
}

void Player::handle_collision() {
    for (int i = 0; i < get_slide_collision_count(); i++) {
        Ref<KinematicCollision2D> collision = get_slide_collision(i);
        if (collision.is_valid()) {
            Object* collider_obj = collision->get_collider();
            Node* collider = Object::cast_to<Node>(collider_obj);
            if (collider && collider->is_in_group("interactable")) {
                set_interactable(collider);
            }
        }
    }
}

void Player::handle_interaction() {
    if (!interaction_ray || !is_interaction_ready()) return;
    
    Input* input = Input::get_singleton();
    if (input->is_action_just_pressed("interact")) {
        Object* collider = interaction_ray->get_collider();
        Node* interactable = Object::cast_to<Node>(collider);
        
        if (interactable && interactable->has_method("interact")) {
            interactable->call("interact");
            interaction_timer = interaction_cooldown;
        }
    }
}

bool Player::is_interaction_ready() const {
    return interaction_timer <= 0;
}

void Player::set_interactable(Node* interactable) {
    if (current_interactable != interactable) {
        if (current_interactable && current_interactable->has_method("on_interaction_end")) {
            current_interactable->call("on_interaction_end");
        }
        
        current_interactable = interactable;
        
        if (current_interactable && current_interactable->has_method("on_interaction_start")) {
            current_interactable->call("on_interaction_start");
        }
    }
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

void Player::set_sprint_multiplier(float p_multiplier) {
    sprint_multiplier = p_multiplier;
}

float Player::get_sprint_multiplier() const {
    return sprint_multiplier;
}

float Player::get_stamina() const {
    return stamina;
}

float Player::get_max_stamina() const {
    return max_stamina;
}

void Player::set_max_stamina(float p_max_stamina) {
    max_stamina = p_max_stamina;
    stamina = Math::min(stamina, max_stamina);
} 