#include "npc.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/shape2d.hpp>

using namespace godot;

void NPC::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_interaction_radius", "radius"), &NPC::set_interaction_radius);
    ClassDB::bind_method(D_METHOD("get_interaction_radius"), &NPC::get_interaction_radius);
    ClassDB::bind_method(D_METHOD("set_patrol_radius", "radius"), &NPC::set_patrol_radius);
    ClassDB::bind_method(D_METHOD("get_patrol_radius"), &NPC::get_patrol_radius);
    ClassDB::bind_method(D_METHOD("set_dialogue_data", "data"), &NPC::set_dialogue_data);
    ClassDB::bind_method(D_METHOD("get_dialogue_data"), &NPC::get_dialogue_data);
    ClassDB::bind_method(D_METHOD("get_is_interactable"), &NPC::get_is_interactable);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &NPC::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &NPC::get_speed);
    ClassDB::bind_method(D_METHOD("interact"), &NPC::interact);
    
    // Register enum
    BIND_ENUM_CONSTANT(IDLE);
    BIND_ENUM_CONSTANT(PATROL);
    BIND_ENUM_CONSTANT(FOLLOW);
    BIND_ENUM_CONSTANT(FLEE);
    BIND_ENUM_CONSTANT(INTERACT);
    BIND_ENUM_CONSTANT(WORK);
    
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
    speed(100.0f),
    patrol_radius(200.0f),
    interaction_radius(100.0f),
    interaction_cooldown(1.0f),
    cooldown_timer(0.0f),
    state_timer(0.0f),
    decision_cooldown(2.0f),
    current_state(NPCState::IDLE),
    current_dialogue_state(0),
    is_interactable(false),
    dialogue_display(nullptr),
    detection_area(nullptr),
    sprite(nullptr),
    personality(nullptr) {
    
    // Create personality profile
    personality = memnew(PersonalityProfile);
}

NPC::~NPC() {
    if (personality) {
        memdelete(personality);
    }
}

void NPC::_init() {
    // Initialize properties
}

void NPC::_ready() {
    home_position = get_position();
    target_position = home_position;
    
    sprite = Object::cast_to<AnimatedSprite2D>(get_node("AnimatedSprite2D"));
    ERR_FAIL_NULL_MSG(sprite, "NPC requires an AnimatedSprite2D child node!");
    
    setup_detection_area();
    setup_dialogue_display();
    
    // Initialize personality with default type if not set
    if (!personality->get_enneagram_type()) {
        initialize_personality(EnneagramType::PEACEMAKER);
    }
}

void NPC::setup_detection_area() {
    detection_area = memnew(Area2D);
    add_child(detection_area);
    
    CollisionShape2D* collision_shape = memnew(CollisionShape2D);
    Ref<CircleShape2D> circle_shape;
    circle_shape.instantiate();
    circle_shape->set_radius(interaction_radius);
    collision_shape->set_shape(circle_shape);
    
    detection_area->add_child(collision_shape);
    
    detection_area->connect("body_entered", callable_mp(this, &NPC::on_player_entered));
    detection_area->connect("body_exited", callable_mp(this, &NPC::on_player_exited));
}

void NPC::setup_dialogue_display() {
    dialogue_display = memnew(Label);
    add_child(dialogue_display);
    dialogue_display->set_position(Vector2(0, -50)); // Position above NPC
    dialogue_display->hide();
}

void NPC::_process(double delta) {
    update_state(delta);
    update_animation();
    
    if (cooldown_timer > 0) {
        cooldown_timer -= delta;
    }
}

void NPC::_physics_process(double delta) {
    handle_movement(delta);
}

void NPC::update_state(double delta) {
    state_timer -= delta;
    decision_cooldown -= delta;
    
    // Update personality
    personality->simulate_growth(delta);
    
    if (decision_cooldown <= 0) {
        make_decision();
        decision_cooldown = 2.0f;
    }
}

void NPC::make_decision() {
    // Get current personality state
    Dictionary state = personality->get_current_state();
    float stress_level = state["stress_level"];
    
    // Calculate state priorities based on personality
    NPCState best_state = current_state;
    float highest_priority = -1;
    
    for (int i = 0; i < 6; i++) {
        NPCState state = static_cast<NPCState>(i);
        float priority = calculate_state_priority(state);
        
        if (priority > highest_priority) {
            highest_priority = priority;
            best_state = state;
        }
    }
    
    if (best_state != current_state) {
        set_state(best_state);
    }
}

float NPC::calculate_state_priority(NPCState state) const {
    Dictionary personality_state = personality->get_current_state();
    float stress_level = personality_state["stress_level"];
    float growth_potential = personality_state["growth_potential"];
    
    float priority = 0;
    
    switch (state) {
        case NPCState::IDLE:
            priority = 0.2f + (1.0f - personality->get_dsm_dimension("negative_affectivity")) * 0.3f;
            break;
            
        case NPCState::PATROL:
            priority = personality->get_dsm_dimension("detachment") * 0.5f;
            if (state_timer <= 0) priority += 0.3f;
            break;
            
        case NPCState::FOLLOW:
            if (is_interactable) {
                priority = (1.0f - personality->get_dsm_dimension("detachment")) * 0.6f;
            }
            break;
            
        case NPCState::FLEE:
            if (is_interactable && stress_level > 0.7f) {
                priority = 0.8f + personality->get_dsm_dimension("negative_affectivity") * 0.2f;
            }
            break;
            
        case NPCState::INTERACT:
            if (is_interactable) {
                priority = (1.0f - personality->get_dsm_dimension("antagonism")) * 0.7f;
            }
            break;
            
        case NPCState::WORK:
            priority = growth_potential * 0.4f;
            break;
    }
    
    return priority;
}

void NPC::handle_movement(double delta) {
    Vector2 direction = Vector2(0, 0);
    
    switch (current_state) {
        case NPCState::PATROL: {
            if (get_position().distance_to(target_position) < 10.0f) {
                target_position = get_random_patrol_point();
            }
            direction = (target_position - get_position()).normalized();
            break;
        }
        
        case NPCState::FOLLOW: {
            Node* player = get_node("/root/Main/Player"); // Adjust path as needed
            if (player) {
                Vector2 player_pos = Object::cast_to<Node2D>(player)->get_position();
                direction = (player_pos - get_position()).normalized();
            }
            break;
        }
        
        case NPCState::FLEE: {
            Node* player = get_node("/root/Main/Player"); // Adjust path as needed
            if (player) {
                Vector2 player_pos = Object::cast_to<Node2D>(player)->get_position();
                direction = (get_position() - player_pos).normalized();
            }
            break;
        }
        
        default:
            break;
    }
    
    if (direction != Vector2(0, 0)) {
        velocity = velocity.move_toward(direction * speed, 500.0f * delta);
    } else {
        velocity = velocity.move_toward(Vector2(0, 0), 500.0f * delta);
    }
    
    set_velocity(velocity);
    move_and_slide();
}

Vector2 NPC::get_random_patrol_point() const {
    float angle = Math::randf() * Math_TAU;
    float distance = Math::randf() * patrol_radius;
    return home_position + Vector2(Math::cos(angle), Math::sin(angle)) * distance;
}

void NPC::update_animation() {
    if (!sprite) return;
    
    String new_animation = "idle";
    bool flip_h = false;
    
    if (velocity.length() > 10.0f) {
        new_animation = "walk";
        flip_h = velocity.x < 0;
    }
    
    if (new_animation != current_animation) {
        sprite->play(new_animation);
        current_animation = new_animation;
    }
    
    sprite->set_flip_h(flip_h);
}

void NPC::interact() {
    if (!is_interactable || cooldown_timer > 0) return;
    
    // Create interaction event
    Dictionary event;
    event["type"] = "interaction";
    event["success"] = true;
    handle_event(event);
    
    if (!dialogue_queue.empty()) {
        display_dialogue(dialogue_queue.front());
        dialogue_queue.pop();
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

void NPC::on_interaction_start() {
    set_state(NPCState::INTERACT);
    
    // Create interaction start event
    Dictionary event;
    event["type"] = "interaction_start";
    handle_event(event);
}

void NPC::on_interaction_end() {
    // Create interaction end event
    Dictionary event;
    event["type"] = "interaction_end";
    handle_event(event);
    
    make_decision();
}

void NPC::display_dialogue(const String& text) {
    dialogue_display->set_text(text);
    dialogue_display->show();
}

void NPC::queue_dialogue(const String& text) {
    dialogue_queue.push(text);
}

void NPC::clear_dialogue() {
    dialogue_display->hide();
    dialogue_display->set_text("");
    while (!dialogue_queue.empty()) {
        dialogue_queue.pop();
    }
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
            interact(); // Display first dialogue immediately
        }
    } else {
        current_dialogue_state = 0; // Reset dialogue state
        clear_dialogue();
    }
}

void NPC::set_state(NPCState new_state) {
    if (current_state == new_state) return;
    
    current_state = new_state;
    state_timer = Math::randf() * 5.0f + 5.0f; // Random duration between 5-10 seconds
    
    // Handle state-specific initialization
    switch (new_state) {
        case NPCState::PATROL:
            target_position = get_random_patrol_point();
            break;
            
        case NPCState::IDLE:
            velocity = Vector2(0, 0);
            break;
            
        default:
            break;
    }
}

NPCState NPC::get_state() const {
    return current_state;
}

void NPC::initialize_personality(EnneagramType type) {
    personality->set_enneagram_type(type);
    
    // Set initial DSM traits based on type
    switch (type) {
        case EnneagramType::REFORMER:
            personality->set_dsm_dimension("negative_affectivity", 0.4f);
            personality->set_dsm_dimension("detachment", 0.3f);
            personality->set_dsm_dimension("antagonism", 0.2f);
            break;
            
        case EnneagramType::HELPER:
            personality->set_dsm_dimension("negative_affectivity", 0.3f);
            personality->set_dsm_dimension("detachment", 0.2f);
            personality->set_dsm_dimension("antagonism", 0.1f);
            break;
            
        // ... Add other types
    }
}

void NPC::handle_event(const Dictionary& event) {
    // Process event through personality system
    personality->process_event(event);
    
    // Update NPC state based on event
    if (event.has("type")) {
        String type = event["type"];
        
        if (type == "threat") {
            Dictionary stressor;
            stressor["intensity"] = event["intensity"];
            personality->handle_stress(stressor);
        } else if (type == "interaction") {
            // Handle interaction events
            if (event.has("success")) {
                bool success = event["success"];
                if (success) {
                    personality->update_emotional_state("satisfaction", 0.6f);
                } else {
                    personality->update_emotional_state("frustration", 0.4f);
                }
            }
        }
    }
}

void NPC::handle_emotional_event(const String& emotion, float intensity) {
    personality->update_emotional_state(emotion, intensity);
}

void NPC::set_personality_trait(const String& trait, float value) {
    personality->set_dsm_dimension(trait, value);
}

float NPC::get_personality_trait(const String& trait) const {
    return personality->get_dsm_dimension(trait);
}

Dictionary NPC::get_personality_state() const {
    return personality->get_current_state();
}

void NPC::set_interaction_radius(float radius) {
    interaction_radius = radius;
    if (detection_area) {
        CollisionShape2D* collision_shape = Object::cast_to<CollisionShape2D>(detection_area->get_child(0));
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

void NPC::set_patrol_radius(float radius) {
    patrol_radius = radius;
}

float NPC::get_patrol_radius() const {
    return patrol_radius;
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

void NPC::set_speed(float p_speed) {
    speed = p_speed;
}

float NPC::get_speed() const {
    return speed;
} 