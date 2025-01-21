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
    if (!personality) return;

    // Update timers
    state_timer += delta;
    cooldown_timer = std::max(0.0f, cooldown_timer - (float)delta);

    // Process personality influence on behavior
    process_personality_influence();

    // Update current state
    update_state(delta);

    // Handle movement based on current state
    handle_movement(delta);

    // Update animations
    update_animation();
}

void NPC::process_personality_influence() {
    if (!personality) return;

    // Get current emotional state
    auto emotional_state = personality->get_emotional_state();
    
    // Adjust behavior based on personality traits
    float aggression = personality->get_trait("aggression");
    float sociability = personality->get_trait("sociability");
    float curiosity = personality->get_trait("curiosity");
    
    // Modify state priorities based on personality
    state_priorities[NPCState::PATROL] = curiosity * 0.8f;
    state_priorities[NPCState::INTERACT] = sociability * 1.2f;
    state_priorities[NPCState::FLEE] = (1.0f - aggression) * 0.9f;
    
    // Apply emotional modifiers
    if (emotional_state["stress"] > 0.7f) {
        state_priorities[NPCState::FLEE] *= 1.5f;
    }
    if (emotional_state["joy"] > 0.6f) {
        state_priorities[NPCState::INTERACT] *= 1.3f;
    }
}

void NPC::update_state(double delta) {
    // Check if it's time to make a new decision
    if (state_timer >= decision_cooldown) {
        make_decision();
        state_timer = 0.0f;
    }

    // Handle state-specific behavior
    switch (current_state) {
        case NPCState::IDLE:
            handle_idle_state(delta);
            break;
        case NPCState::PATROL:
            handle_patrol_state(delta);
            break;
        case NPCState::FOLLOW:
            handle_follow_state(delta);
            break;
        case NPCState::FLEE:
            handle_flee_state(delta);
            break;
        case NPCState::INTERACT:
            handle_interact_state(delta);
            break;
        case NPCState::WORK:
            handle_work_state(delta);
            break;
    }
}

void NPC::make_decision() {
    // Get current context
    auto nearby_entities = get_nearby_entities();
    auto current_location = get_global_position();
    auto emotional_state = personality->get_emotional_state();

    // Calculate priorities for each state
    std::map<NPCState, float> priorities;
    for (const auto& pair : state_priorities) {
        float base_priority = pair.second;
        float context_modifier = calculate_context_priority(pair.first, nearby_entities, current_location);
        float emotional_modifier = calculate_emotional_priority(pair.first, emotional_state);
        
        priorities[pair.first] = base_priority * context_modifier * emotional_modifier;
    }

    // Select highest priority state
    NPCState best_state = NPCState::IDLE;
    float highest_priority = 0.0f;
    
    for (const auto& pair : priorities) {
        if (pair.second > highest_priority) {
            highest_priority = pair.second;
            best_state = pair.first;
        }
    }

    // Set new state if different from current
    if (best_state != current_state) {
        set_state(best_state);
    }
}

float NPC::calculate_context_priority(NPCState state, const Array& nearby_entities, const Vector2& current_location) {
    float priority = 1.0f;

    switch (state) {
        case NPCState::PATROL:
            // Higher priority when no interesting entities nearby
            priority = nearby_entities.is_empty() ? 1.5f : 0.8f;
            break;
            
        case NPCState::FOLLOW:
            // Higher priority when player is nearby
            for (int i = 0; i < nearby_entities.size(); i++) {
                Node* entity = Object::cast_to<Node>(nearby_entities[i]);
                if (entity && entity->is_in_group("player")) {
                    priority = 1.8f;
                    break;
                }
            }
            break;
            
        case NPCState::FLEE:
            // Higher priority when threats are nearby
            for (int i = 0; i < nearby_entities.size(); i++) {
                Node* entity = Object::cast_to<Node>(nearby_entities[i]);
                if (entity && entity->is_in_group("threat")) {
                    priority = 2.0f;
                    break;
                }
            }
            break;
            
        case NPCState::INTERACT:
            // Higher priority when interactable entities are nearby
            for (int i = 0; i < nearby_entities.size(); i++) {
                Node* entity = Object::cast_to<Node>(nearby_entities[i]);
                if (entity && entity->is_in_group("interactable")) {
                    priority = 1.6f;
                    break;
                }
            }
            break;
            
        case NPCState::WORK:
            // Higher priority during work hours or near workstation
            if (is_work_hours() || is_near_workstation(current_location)) {
                priority = 1.7f;
            }
            break;
            
        default:
            break;
    }

    return priority;
}

float NPC::calculate_emotional_priority(NPCState state, const Dictionary& emotional_state) {
    float priority = 1.0f;
    
    // Get relevant emotional values
    float stress = emotional_state["stress"];
    float joy = emotional_state["joy"];
    float fatigue = emotional_state["fatigue"];
    
    switch (state) {
        case NPCState::IDLE:
            // More likely to idle when fatigued
            priority = 1.0f + (fatigue * 0.5f);
            break;
            
        case NPCState::PATROL:
            // Less likely to patrol when stressed or fatigued
            priority = 1.0f - (stress * 0.3f) - (fatigue * 0.2f);
            break;
            
        case NPCState::FOLLOW:
            // More likely to follow when joyful
            priority = 1.0f + (joy * 0.4f);
            break;
            
        case NPCState::FLEE:
            // Much more likely to flee when stressed
            priority = 1.0f + (stress * 0.8f);
            break;
            
        case NPCState::INTERACT:
            // More likely to interact when joyful, less when stressed
            priority = 1.0f + (joy * 0.5f) - (stress * 0.3f);
            break;
            
        case NPCState::WORK:
            // Less likely to work when fatigued or stressed
            priority = 1.0f - (fatigue * 0.4f) - (stress * 0.2f);
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

void NPC::handle_idle_state(double delta) {
    // In idle state, the NPC should occasionally look around
    if (state_timer > 3.0f) {
        // Random rotation to simulate looking around
        sprite->set_flip_h(Math::randf() > 0.5f);
        state_timer = 0.0f;
    }
    
    // Slow down any existing movement
    velocity = velocity.lerp(Vector2(), delta * 2.0f);
}

void NPC::handle_patrol_state(double delta) {
    if (get_distance_to_target() < 10.0f || state_timer > 10.0f) {
        // Get new patrol point when close to target or timeout
        target_position = get_random_patrol_point();
        state_timer = 0.0f;
    }
    
    // Calculate path to target
    Vector2 direction = (target_position - get_global_position()).normalized();
    Vector2 desired_velocity = direction * speed;
    
    // Apply steering behavior
    Vector2 avoidance = calculate_avoidance_vector();
    desired_velocity += avoidance;
    
    // Update velocity with smoothing
    velocity = velocity.lerp(desired_velocity, delta * 3.0f);
}

void NPC::handle_follow_state(double delta) {
    Array nearby = get_nearby_entities();
    Node* target = nullptr;
    
    // Find closest player or interesting entity
    float closest_dist = 1000000.0f;
    for (int i = 0; i < nearby.size(); i++) {
        Node* entity = Object::cast_to<Node>(nearby[i]);
        if (entity && (entity->is_in_group("player") || entity->is_in_group("interesting"))) {
            float dist = get_global_position().distance_to(Object::cast_to<Node2D>(entity)->get_global_position());
            if (dist < closest_dist) {
                closest_dist = dist;
                target = entity;
            }
        }
    }
    
    if (target) {
        // Update target position
        target_position = Object::cast_to<Node2D>(target)->get_global_position();
        
        // Calculate direction while maintaining some distance
        Vector2 direction = (target_position - get_global_position()).normalized();
        float target_distance = get_global_position().distance_to(target_position);
        
        if (target_distance > interaction_radius * 1.5f) {
            // Move closer if too far
            velocity = velocity.lerp(direction * speed, delta * 3.0f);
        } else if (target_distance < interaction_radius * 0.8f) {
            // Back up if too close
            velocity = velocity.lerp(-direction * speed * 0.5f, delta * 3.0f);
        } else {
            // Maintain distance
            velocity = velocity.lerp(Vector2(), delta * 3.0f);
        }
    } else {
        // No target found, return to idle
        set_state(NPCState::IDLE);
    }
}

void NPC::handle_flee_state(double delta) {
    Array nearby = get_nearby_entities();
    Vector2 flee_direction = Vector2();
    
    // Calculate flee direction away from threats
    for (int i = 0; i < nearby.size(); i++) {
        Node* entity = Object::cast_to<Node>(nearby[i]);
        if (entity && entity->is_in_group("threat")) {
            Vector2 threat_pos = Object::cast_to<Node2D>(entity)->get_global_position();
            Vector2 away_dir = (get_global_position() - threat_pos).normalized();
            flee_direction += away_dir;
        }
    }
    
    if (flee_direction != Vector2()) {
        // Normalize and apply speed
        flee_direction = flee_direction.normalized();
        Vector2 desired_velocity = flee_direction * speed * 1.5f; // Flee faster than normal speed
        
        // Add some randomness to make the movement more panicked
        desired_velocity += Vector2(Math::randf() * 0.2f - 0.1f, Math::randf() * 0.2f - 0.1f) * speed;
        
        // Update velocity with quick response
        velocity = velocity.lerp(desired_velocity, delta * 5.0f);
    } else {
        // No threats nearby, calm down
        set_state(NPCState::IDLE);
    }
}

void NPC::handle_interact_state(double delta) {
    if (!is_interactable) {
        set_state(NPCState::IDLE);
        return;
    }
    
    // Face the interaction target
    sprite->set_flip_h(target_position.x < get_global_position().x);
    
    // Stay relatively still during interaction
    velocity = velocity.lerp(Vector2(), delta * 4.0f);
    
    // Process dialogue if any
    if (!dialogue_queue.empty() && state_timer > 2.0f) {
        display_dialogue(dialogue_queue.front());
        dialogue_queue.pop();
        state_timer = 0.0f;
    }
}

void NPC::handle_work_state(double delta) {
    if (!is_work_hours() && !is_near_workstation(get_global_position())) {
        set_state(NPCState::IDLE);
        return;
    }
    
    // Move to workstation if not there
    if (!is_near_workstation(get_global_position())) {
        Vector2 workstation_pos = get_nearest_workstation();
        Vector2 direction = (workstation_pos - get_global_position()).normalized();
        velocity = velocity.lerp(direction * speed * 0.7f, delta * 2.0f);
    } else {
        // Perform work animations/actions
        velocity = velocity.lerp(Vector2(), delta * 3.0f);
        // TODO: Implement work animations
    }
}

Array NPC::get_nearby_entities() const {
    Array nearby;
    if (detection_area) {
        nearby = detection_area->get_overlapping_bodies();
    }
    return nearby;
}

bool NPC::is_work_hours() const {
    // TODO: Implement actual time system
    return true; // Temporary always true
}

bool NPC::is_near_workstation(const Vector2& location) const {
    // TODO: Implement actual workstation detection
    return false; // Temporary always false
}

float NPC::get_distance_to_target() const {
    return get_global_position().distance_to(target_position);
}

Vector2 NPC::calculate_avoidance_vector() const {
    Vector2 avoidance;
    Array nearby = get_nearby_entities();
    
    for (int i = 0; i < nearby.size(); i++) {
        Node* entity = Object::cast_to<Node>(nearby[i]);
        if (entity && entity != this) {
            Vector2 entity_pos = Object::cast_to<Node2D>(entity)->get_global_position();
            Vector2 to_entity = entity_pos - get_global_position();
            float distance = to_entity.length();
            
            if (distance < interaction_radius) {
                // Add avoidance force inversely proportional to distance
                avoidance -= to_entity.normalized() * (interaction_radius - distance) / interaction_radius;
            }
        }
    }
    
    return avoidance.normalized() * speed * 0.5f;
}

bool NPC::has_line_of_sight(const Vector2& target) const {
    // TODO: Implement actual line of sight checking with raycasts
    return true; // Temporary always true
}

Vector2 NPC::get_nearest_workstation() const {
    // TODO: Implement actual workstation finding
    return home_position; // Temporary return home position
} 