#include "npc.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/shape2d.hpp>

using namespace godot;

void NPC::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_interaction_radius", "radius"), &NPC::set_interaction_radius);
    ClassDB::bind_method(D_METHOD("get_interaction_radius"), &NPC::get_interaction_radius);
    ClassDB::bind_method(D_METHOD("set_dialogue_data", "data"), &NPC::set_dialogue_data);
    ClassDB::bind_method(D_METHOD("get_dialogue_data"), &NPC::get_dialogue_data);
    ClassDB::bind_method(D_METHOD("get_is_interactable"), &NPC::get_is_interactable);
    ClassDB::bind_method(D_METHOD("handle_interaction"), &NPC::handle_interaction);
    ClassDB::bind_method(D_METHOD("on_player_entered", "body"), &NPC::on_player_entered);
    ClassDB::bind_method(D_METHOD("on_player_exited", "body"), &NPC::on_player_exited);
    
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "interaction_radius"), "set_interaction_radius", "get_interaction_radius");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "dialogue_data"), "set_dialogue_data", "get_dialogue_data");
}

NPC::NPC() {
    is_interactable = false;
    interaction_radius = 100.0f;
    interaction_cooldown = 1.0f;
    cooldown_timer = 0.0f;
    current_dialogue_state = 0;
    dialogue_display = nullptr;
    interaction_area = nullptr;
}

NPC::~NPC() {
    // Cleanup if needed
}

void NPC::_init() {
    // Initialize properties
}

void NPC::_ready() {
    setup_interaction_area();
    setup_dialogue_display();
}

void NPC::setup_interaction_area() {
    interaction_area = memnew(Area2D);
    add_child(interaction_area);
    
    CollisionShape2D* collision_shape = memnew(CollisionShape2D);
    Ref<CircleShape2D> circle_shape;
    circle_shape.instantiate();
    circle_shape->set_radius(interaction_radius);
    collision_shape->set_shape(circle_shape);
    
    interaction_area->add_child(collision_shape);
    
    interaction_area->connect("body_entered", callable_mp(this, &NPC::on_player_entered));
    interaction_area->connect("body_exited", callable_mp(this, &NPC::on_player_exited));
}

void NPC::setup_dialogue_display() {
    dialogue_display = memnew(Label);
    add_child(dialogue_display);
    dialogue_display->set_position(Vector2(0, -50)); // Position above NPC
    dialogue_display->hide();
}

void NPC::_process(double delta) {
    if (cooldown_timer > 0) {
        cooldown_timer -= delta;
    }
}

void NPC::handle_interaction() {
    if (!is_interactable || cooldown_timer > 0) return;
    
    if (!dialogue_queue.is_empty()) {
        String next_dialogue = dialogue_queue[0];
        dialogue_queue.remove_at(0);
        display_dialogue(next_dialogue);
    } else {
        advance_dialogue_state();
    }
    
    cooldown_timer = interaction_cooldown;
}

void NPC::on_player_entered(Node* body) {
    if (body->is_in_group("player")) {
        is_interactable = true;
    }
}

void NPC::on_player_exited(Node* body) {
    if (body->is_in_group("player")) {
        is_interactable = false;
        clear_dialogue();
    }
}

void NPC::display_dialogue(const String& text) {
    dialogue_display->set_text(text);
    dialogue_display->show();
}

void NPC::queue_dialogue(const String& text) {
    dialogue_queue.push_back(text);
}

void NPC::clear_dialogue() {
    dialogue_display->hide();
    dialogue_display->set_text("");
    dialogue_queue.clear();
}

void NPC::advance_dialogue_state() {
    current_dialogue_state++;
    if (dialogue_data.has(String::num(current_dialogue_state))) {
        Variant next_dialogue = dialogue_data[String::num(current_dialogue_state)];
        if (next_dialogue.get_type() == Variant::STRING) {
            display_dialogue(next_dialogue);
        } else if (next_dialogue.get_type() == Variant::ARRAY) {
            Array dialogue_array = next_dialogue;
            for (int i = 0; i < dialogue_array.size(); i++) {
                queue_dialogue(dialogue_array[i]);
            }
            handle_interaction(); // Display first dialogue immediately
        }
    } else {
        current_dialogue_state = 0; // Reset dialogue state
        clear_dialogue();
    }
}

void NPC::set_interaction_radius(float radius) {
    interaction_radius = radius;
    if (interaction_area) {
        CollisionShape2D* collision_shape = Object::cast_to<CollisionShape2D>(interaction_area->get_child(0));
        if (collision_shape) {
            Ref<Shape2D> shape = collision_shape->get_shape();
            Ref<CircleShape2D> circle_shape = shape;
            if (circle_shape.is_valid()) {
                circle_shape->set_radius(radius);
            }
        }
    }
}

float NPC::get_interaction_radius() const {
    return interaction_radius;
}

void NPC::set_dialogue_data(const Dictionary& data) {
    dialogue_data = data;
}

Dictionary NPC::get_dialogue_data() const {
    return dialogue_data;
}

bool NPC::get_is_interactable() const {
    return is_interactable;
} 