#ifndef NPC_H
#define NPC_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class NPC : public Node2D {
    GDCLASS(NPC, Node2D)

private:
    bool is_interactable;
    Label* dialogue_display;
    Area2D* interaction_area;
    float interaction_radius;
    float interaction_cooldown;
    float cooldown_timer;
    
    // Dialogue state
    int current_dialogue_state;
    Dictionary dialogue_data;
    Array dialogue_queue;
    
    void setup_interaction_area();
    void setup_dialogue_display();

protected:
    static void _bind_methods();

public:
    NPC();
    ~NPC();

    void _init();
    void _ready();
    void _process(double delta);
    
    // Interaction methods
    void handle_interaction();
    void on_player_entered(Node* body);
    void on_player_exited(Node* body);
    
    // Dialogue methods
    void display_dialogue(const String& text);
    void queue_dialogue(const String& text);
    void clear_dialogue();
    void advance_dialogue_state();
    
    // Getters and setters
    void set_interaction_radius(float radius);
    float get_interaction_radius() const;
    void set_dialogue_data(const Dictionary& data);
    Dictionary get_dialogue_data() const;
    bool get_is_interactable() const;
};

}

#endif // NPC_H 