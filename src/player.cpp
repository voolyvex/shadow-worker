#include "player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/classes/input.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Player::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_max_speed", "speed"), &Player::set_max_speed);
    ClassDB::bind_method(D_METHOD("get_max_speed"), &Player::get_max_speed);
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
    sprite = get_node<AnimatedSprite2D>("AnimatedSprite2D");
    camera = get_node<Camera2D>("Camera2D");
    
    if (!sprite) {
        UtilityFunctions::print("Warning: AnimatedSprite2D node not found");
    }
    
    if (camera) {
        camera->make_current();
    }
    
    // Initialize movement variables
    velocity = Vector2(0, 0);
    speed = base_speed;
    stamina = max_stamina;
}

void Player::_process(double delta) {
    Input* input = Input::get_singleton();
    
    // Get input direction
    Vector2 input_direction;
    input_direction.x = input->get_action_strength("move_right") - input->get_action_strength("move_left");
    input_direction.y = input->get_action_strength("move_down") - input->get_action_strength("move_up");
    
    // Normalize input direction
    if (input_direction.length() > 0) {
        input_direction = input_direction.normalized();
    }
    
    // Handle sprinting and stamina
    bool is_sprinting = input->is_action_pressed("sprint") && stamina > 0;
    float target_speed = is_sprinting ? sprint_speed : base_speed;
    
    if (is_sprinting && input_direction.length() > 0) {
        stamina = Math::clamp(stamina - static_cast<float>(delta) * stamina_drain_rate, 0.0f, max_stamina);
    } else {
        stamina = Math::clamp(stamina + static_cast<float>(delta) * stamina_regen_rate, 0.0f, max_stamina);
    }
    
    // Update velocity with linear interpolation
    Vector2 target_velocity = input_direction * target_speed;
    velocity = velocity.lerp(target_velocity, static_cast<float>(delta) * acceleration);
    
    // Update position
    set_position(get_position() + velocity * static_cast<float>(delta));
    
    // Update animation
    if (sprite) {
        String anim_name = velocity.length() > 0 ? "walk" : "idle";
        if (current_animation != anim_name) {
            current_animation = anim_name;
            sprite->set_flip_h(velocity.x < 0);
        }
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
        stamina = Math::max<float>(0.0f, stamina - stamina_drain_rate * delta);
    } else if (stamina < max_stamina) {
        stamina = Math::min<float>(max_stamina, stamina + stamina_regen_rate * delta);
    }
}

void Player::update_animation() {
    if (!sprite) return;
    
    String anim_name = "idle";
    bool flip_h = false;
    
    if (velocity.length() > 10.0f) {
        anim_name = "walk";
        flip_h = velocity.x < 0;
    }
    
    if (anim_name != current_animation) {
        sprite->play(anim_name);
        current_animation = anim_name;
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
    return interaction_timer <= 0.0f;
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
    base_speed = p_speed;
}

float Player::get_speed() const {
    return base_speed;
}

void Player::set_max_speed(float p_speed) {
    sprint_speed = p_speed;
}

float Player::get_max_speed() const {
    return sprint_speed;
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