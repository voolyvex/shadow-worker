#include "personality_system.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void PersonalityProfile::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_enneagram_type", "type"), &PersonalityProfile::set_enneagram_type);
    ClassDB::bind_method(D_METHOD("get_enneagram_type"), &PersonalityProfile::get_enneagram_type);
    ClassDB::bind_method(D_METHOD("add_wing", "wing"), &PersonalityProfile::add_wing);
    ClassDB::bind_method(D_METHOD("set_development_level", "level"), &PersonalityProfile::set_development_level);
    ClassDB::bind_method(D_METHOD("set_dsm_dimension", "dimension", "value"), &PersonalityProfile::set_dsm_dimension);
    ClassDB::bind_method(D_METHOD("get_dsm_dimension", "dimension"), &PersonalityProfile::get_dsm_dimension);
    ClassDB::bind_method(D_METHOD("add_behavior_pattern", "behavior", "weight", "context"), &PersonalityProfile::add_behavior_pattern);
    ClassDB::bind_method(D_METHOD("get_behavior_patterns"), &PersonalityProfile::get_behavior_patterns);
    ClassDB::bind_method(D_METHOD("update_emotional_state", "emotion", "intensity"), &PersonalityProfile::update_emotional_state);
    ClassDB::bind_method(D_METHOD("get_emotional_intensity", "emotion"), &PersonalityProfile::get_emotional_intensity);
    ClassDB::bind_method(D_METHOD("process_event", "event"), &PersonalityProfile::process_event);
    ClassDB::bind_method(D_METHOD("get_current_state"), &PersonalityProfile::get_current_state);
    ClassDB::bind_method(D_METHOD("simulate_growth", "delta"), &PersonalityProfile::simulate_growth);
    ClassDB::bind_method(D_METHOD("handle_stress", "stressor"), &PersonalityProfile::handle_stress);
    ClassDB::bind_method(D_METHOD("get_growth_opportunities"), &PersonalityProfile::get_growth_opportunities);
    ClassDB::bind_method(D_METHOD("to_dictionary"), &PersonalityProfile::to_dictionary);
    ClassDB::bind_method(D_METHOD("from_dictionary", "data"), &PersonalityProfile::from_dictionary);
    ClassDB::bind_method(D_METHOD("add_shadow_aspect", "trait", "intensity", "is_positive"), &PersonalityProfile::add_shadow_aspect);
    ClassDB::bind_method(D_METHOD("process_projection", "trait", "target"), &PersonalityProfile::process_projection);
    ClassDB::bind_method(D_METHOD("get_archetype_influence", "archetype"), &PersonalityProfile::get_archetype_influence);
    ClassDB::bind_method(D_METHOD("get_active_projections"), &PersonalityProfile::get_active_projections);
    ClassDB::bind_method(D_METHOD("add_surreal_state", "symbol", "distortion"), &PersonalityProfile::add_surreal_state);
    ClassDB::bind_method(D_METHOD("get_symbolic_state"), &PersonalityProfile::get_symbolic_state);
    ClassDB::bind_method(D_METHOD("get_reality_distortion"), &PersonalityProfile::get_reality_distortion);
    ClassDB::bind_method(D_METHOD("get_individuation_progress"), &PersonalityProfile::get_individuation_progress);
    ClassDB::bind_method(D_METHOD("process_blood_resonance", "intensity"), &PersonalityProfile::process_blood_resonance);
    ClassDB::bind_method(D_METHOD("update_beast_state", "delta"), &PersonalityProfile::update_beast_state);
    ClassDB::bind_method(D_METHOD("handle_cosmic_insight", "revelation"), &PersonalityProfile::handle_cosmic_insight);
    ClassDB::bind_method(D_METHOD("update_blood_corruption", "delta"), &PersonalityProfile::update_blood_corruption);
    ClassDB::bind_method(D_METHOD("process_cosmic_revelation"), &PersonalityProfile::process_cosmic_revelation);
    ClassDB::bind_method(D_METHOD("integrate_blood_memory", "memory"), &PersonalityProfile::integrate_blood_memory);
    ClassDB::bind_method(D_METHOD("calculate_transformation_threshold"), &PersonalityProfile::calculate_transformation_threshold);
    ClassDB::bind_method(D_METHOD("trigger_beast_transformation", "catalyst_strength"), &PersonalityProfile::trigger_beast_transformation);
    ClassDB::bind_method(D_METHOD("resist_transformation", "willpower"), &PersonalityProfile::resist_transformation);
    ClassDB::bind_method(D_METHOD("get_beast_state"), &PersonalityProfile::get_beast_state);
    ClassDB::bind_method(D_METHOD("get_insight_level"), &PersonalityProfile::get_insight_level);
    ClassDB::bind_method(D_METHOD("get_blood_memories"), &PersonalityProfile::get_blood_memories);
    ClassDB::bind_method(D_METHOD("has_eyes_on_the_inside"), &PersonalityProfile::has_eyes_on_the_inside);
    ClassDB::bind_method(D_METHOD("get_cosmic_state"), &PersonalityProfile::get_cosmic_state);
    ClassDB::bind_method(D_METHOD("process_resonance", "intensity"), &PersonalityProfile::process_resonance);
    ClassDB::bind_method(D_METHOD("update_perception_layer", "delta"), &PersonalityProfile::update_perception_layer);
    ClassDB::bind_method(D_METHOD("handle_inner_revelation", "revelation"), &PersonalityProfile::handle_inner_revelation);
    ClassDB::bind_method(D_METHOD("calculate_consciousness_threshold"), &PersonalityProfile::calculate_consciousness_threshold);
    ClassDB::bind_method(D_METHOD("trigger_reality_shift", "catalyst_strength"), &PersonalityProfile::trigger_reality_shift);
    ClassDB::bind_method(D_METHOD("resist_dissolution", "willpower"), &PersonalityProfile::resist_dissolution);
    ClassDB::bind_method(D_METHOD("get_resonance_state"), &PersonalityProfile::get_resonance_state);
    ClassDB::bind_method(D_METHOD("get_consciousness_level"), &PersonalityProfile::get_consciousness_level);
    ClassDB::bind_method(D_METHOD("get_psychological_echoes"), &PersonalityProfile::get_psychological_echoes);
    ClassDB::bind_method(D_METHOD("has_reality_shaping"), &PersonalityProfile::has_reality_shaping);
    ClassDB::bind_method(D_METHOD("get_inner_state"), &PersonalityProfile::get_inner_state);
}

PersonalityProfile::PersonalityProfile() :
    primary_type(EnneagramType::PEACEMAKER),
    stress_level(0.0f),
    growth_potential(1.0f),
    individuation_progress(0.0f),
    reality_distortion_level(0.0f) {
    
    // Initialize DSM dimensions to neutral values
    dsm_traits = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    dsm_traits.shadow_integration = 0.0f;
    
    // Initialize archetype influences
    for (int i = 0; i < 9; i++) {
        archetype_influence[static_cast<JungianArchetype>(i)] = 0.0f;
    }
    
    // Set initial development level
    development.level = 5;
    development.description = "Average health level";
    development.shadow_integration_level = 0.3f;
}

PersonalityProfile::~PersonalityProfile() {
    // Cleanup if needed
}

void PersonalityProfile::set_enneagram_type(EnneagramType type) {
    primary_type = type;
    
    // Set default core traits and shadow aspects based on type
    core_traits.fears.clear();
    core_traits.desires.clear();
    core_traits.defense_mechanisms.clear();
    core_traits.repressed_qualities.clear();
    
    switch (type) {
        case EnneagramType::REFORMER: {
            core_traits.fears = {"corruption", "imbalance", "imperfection"};
            core_traits.desires = {"integrity", "balance", "improvement"};
            core_traits.defense_mechanisms = {"reaction formation", "rationalization"};
            
            // Shadow aspects specific to Type 1
            add_shadow_aspect("chaos", 0.7f, false);
            add_shadow_aspect("spontaneity", 0.6f, true);
            
            // Surreal manifestations
            add_surreal_state("perfect_geometry", 0.3f);
            break;
        }
        case EnneagramType::HELPER: {
            core_traits.fears = {"unworthiness", "being unloved", "neediness"};
            core_traits.desires = {"being loved", "expressing love", "appreciation"};
            core_traits.defense_mechanisms = {"repression", "denial"};
            
            // Shadow aspects specific to Type 2
            add_shadow_aspect("selfishness", 0.8f, false);
            add_shadow_aspect("self-care", 0.5f, true);
            
            // Surreal manifestations
            add_surreal_state("mirror_reflection", 0.4f);
            break;
        }
        // ... Add other types with their shadow aspects
    }
    
    update_development_level();
    integrate_shadow_aspects();
}

EnneagramType PersonalityProfile::get_enneagram_type() const {
    return primary_type;
}

void PersonalityProfile::add_wing(EnneagramType wing) {
    // Only allow adjacent types as wings
    int primary_num = static_cast<int>(primary_type);
    int wing_num = static_cast<int>(wing);
    
    if (std::abs(primary_num - wing_num) == 1 || 
        (primary_num == 9 && wing_num == 1) || 
        (primary_num == 1 && wing_num == 9)) {
        wings.push_back(wing);
    }
}

void PersonalityProfile::set_development_level(int level) {
    development.level = Math::clamp(level, 1, 9);
    update_development_level();
}

void PersonalityProfile::update_development_level() {
    // Update behavior patterns based on health level
    development.characteristic_behaviors.clear();
    
    if (development.level <= 3) {  // Healthy levels
        development.description = "Healthy - Self-actualizing";
        add_behavior_pattern("self_awareness", 0.8f, Dictionary());
        add_behavior_pattern("growth_oriented", 0.7f, Dictionary());
    } else if (development.level <= 6) {  // Average levels
        development.description = "Average - Coping";
        add_behavior_pattern("defensive", 0.6f, Dictionary());
        add_behavior_pattern("compensating", 0.5f, Dictionary());
    } else {  // Unhealthy levels
        development.description = "Unhealthy - Reactive";
        add_behavior_pattern("destructive", 0.7f, Dictionary());
        add_behavior_pattern("neurotic", 0.6f, Dictionary());
    }
}

void PersonalityProfile::set_dsm_dimension(const String& dimension, float value) {
    value = Math::clamp(value, 0.0f, 1.0f);
    
    if (dimension == "negative_affectivity") dsm_traits.negative_affectivity = value;
    else if (dimension == "detachment") dsm_traits.detachment = value;
    else if (dimension == "antagonism") dsm_traits.antagonism = value;
    else if (dimension == "disinhibition") dsm_traits.disinhibition = value;
    else if (dimension == "psychoticism") dsm_traits.psychoticism = value;
}

float PersonalityProfile::get_dsm_dimension(const String& dimension) const {
    if (dimension == "negative_affectivity") return dsm_traits.negative_affectivity;
    else if (dimension == "detachment") return dsm_traits.detachment;
    else if (dimension == "antagonism") return dsm_traits.antagonism;
    else if (dimension == "disinhibition") return dsm_traits.disinhibition;
    else if (dimension == "psychoticism") return dsm_traits.psychoticism;
    return 0.0f;
}

void PersonalityProfile::add_behavior_pattern(const String& behavior, float weight, const Dictionary& context) {
    BehaviorPattern pattern{behavior, weight, context};
    development.characteristic_behaviors.push_back(pattern);
}

Array PersonalityProfile::get_behavior_patterns() const {
    Array patterns;
    for (const auto& pattern : development.characteristic_behaviors) {
        Dictionary p;
        p["behavior"] = pattern.behavior;
        p["weight"] = pattern.weight;
        p["context"] = pattern.context;
        patterns.push_back(p);
    }
    return patterns;
}

float PersonalityProfile::calculate_behavior_probability(const String& behavior, const Dictionary& context) const {
    float base_probability = 0.0f;
    
    // Find matching behavior pattern
    for (const auto& pattern : development.characteristic_behaviors) {
        if (pattern.behavior == behavior) {
            base_probability = pattern.weight;
            break;
        }
    }
    
    // Modify based on emotional state
    float emotional_modifier = 1.0f;
    for (const auto& [emotion, intensity] : emotional_state) {
        // Apply emotional influence based on context
        if (context.has("emotions") && context["emotions"].operator Array().has(emotion)) {
            emotional_modifier *= (1.0f + intensity);
        }
    }
    
    // Consider stress level
    float stress_modifier = 1.0f + (stress_level * 0.5f);
    
    return Math::clamp(base_probability * emotional_modifier * stress_modifier, 0.0f, 1.0f);
}

void PersonalityProfile::update_emotional_state(const String& emotion, float intensity) {
    emotional_state[emotion] = Math::clamp(intensity, 0.0f, 1.0f);
    process_stress_response();
}

float PersonalityProfile::get_emotional_intensity(const String& emotion) const {
    auto it = emotional_state.find(emotion);
    return it != emotional_state.end() ? it->second : 0.0f;
}

void PersonalityProfile::process_event(const Dictionary& event) {
    // Process impact on emotional state
    if (event.has("emotions")) {
        Array emotions = event["emotions"];
        for (int i = 0; i < emotions.size(); i++) {
            Dictionary emotion = emotions[i];
            update_emotional_state(emotion["type"], emotion["intensity"]);
        }
    }
    
    // Update behavioral history
    if (event.has("behavior")) {
        String behavior = event["behavior"];
        if (!behavioral_history.has(behavior)) {
            behavioral_history[behavior] = Array();
        }
        Array history = behavioral_history[behavior];
        history.push_back(event);
        behavioral_history[behavior] = history;
    }
    
    // Process stress impact
    if (event.has("stress_impact")) {
        float impact = event["stress_impact"];
        stress_level = Math::clamp(stress_level + impact, 0.0f, 1.0f);
        process_stress_response();
    }
    
    // Process blood and cosmic events
    if (event.has("blood_resonance")) {
        process_blood_resonance(event["blood_resonance"]);
    }
    
    if (event.has("cosmic_insight")) {
        handle_cosmic_insight(event["cosmic_insight"]);
    }
}

void PersonalityProfile::process_stress_response() {
    if (stress_level > 0.7f) {
        // Move towards stress direction
        int stress_point = static_cast<int>(primary_type);
        switch (stress_point) {
            case 1: stress_point = 4; break;
            case 2: stress_point = 8; break;
            case 3: stress_point = 9; break;
            case 4: stress_point = 2; break;
            case 5: stress_point = 7; break;
            case 6: stress_point = 3; break;
            case 7: stress_point = 1; break;
            case 8: stress_point = 5; break;
            case 9: stress_point = 6; break;
        }
        
        // Temporarily adopt stress point characteristics
        development.level = Math::min(development.level + 1, 9);
        update_development_level();
    }
}

void PersonalityProfile::add_shadow_aspect(const String& trait, float intensity, bool is_positive) {
    ShadowAspect aspect;
    aspect.trait = trait;
    aspect.intensity = Math::clamp(intensity, 0.0f, 1.0f);
    aspect.is_positive = is_positive;
    
    Dictionary context;
    context["stress_threshold"] = 0.7f;
    context["growth_threshold"] = 0.3f;
    aspect.context = context;
    
    core_traits.repressed_qualities.push_back(aspect);
}

void PersonalityProfile::process_projection(const String& trait, Node* target) {
    // Find matching shadow aspect
    for (const auto& aspect : core_traits.repressed_qualities) {
        if (aspect.trait == trait) {
            // Create projection event
            Dictionary projection;
            projection["trait"] = trait;
            projection["intensity"] = aspect.intensity;
            projection["is_positive"] = aspect.is_positive;
            projection["target"] = target;
            
            active_projections.push_back(aspect);
            
            // Update individuation progress
            if (aspect.is_positive) {
                individuation_progress = Math::min(1.0f, individuation_progress + 0.1f);
            }
            
            break;
        }
    }
}

void PersonalityProfile::add_surreal_state(const String& symbol, float distortion) {
    SurrealState state;
    state.symbol = symbol;
    state.distortion = Math::clamp(distortion, 0.0f, 1.0f);
    
    // Associate with appropriate archetype
    if (symbol.contains("mirror")) {
        state.archetype = "SHADOW";
    } else if (symbol.contains("mother")) {
        state.archetype = "GREAT_MOTHER";
    } // ... etc
    
    Dictionary metaphor;
    metaphor["primary_meaning"] = "unconscious_reflection";
    metaphor["secondary_meaning"] = "hidden_truth";
    state.metaphor = metaphor;
    
    current_symbolic_state.push_back(state);
    reality_distortion_level = Math::max(reality_distortion_level, distortion);
}

void PersonalityProfile::integrate_shadow_aspects() {
    float total_integration = 0.0f;
    
    for (const auto& aspect : core_traits.repressed_qualities) {
        if (aspect.is_positive) {
            total_integration += aspect.intensity * 0.2f;
        }
    }
    
    dsm_traits.shadow_integration = Math::clamp(total_integration, 0.0f, 1.0f);
    development.shadow_integration_level = dsm_traits.shadow_integration;
    
    // Update reality distortion based on integration
    reality_distortion_level = Math::max(0.0f, 1.0f - dsm_traits.shadow_integration);
}

void PersonalityProfile::update_symbolic_state() {
    // Clear old states
    current_symbolic_state.clear();
    
    // Add new states based on current psychological state
    if (stress_level > 0.7f) {
        add_surreal_state("red_room", 0.8f);
        add_surreal_state("flickering_lights", 0.6f);
    }
    
    if (development.level <= 3) {
        add_surreal_state("golden_light", 0.3f);
        add_surreal_state("clear_water", 0.2f);
    }
    
    // Add shadow-related symbols
    if (dsm_traits.shadow_integration < 0.3f) {
        add_surreal_state("dark_corridor", 0.7f);
        add_surreal_state("twisted_reflection", 0.9f);
    }
}

void PersonalityProfile::process_archetype_dynamics() {
    // Update archetype influences based on current state
    for (auto& [archetype, influence] : archetype_influence) {
        switch (archetype) {
            case JungianArchetype::SHADOW:
                influence = 1.0f - dsm_traits.shadow_integration;
                break;
            case JungianArchetype::SELF:
                influence = individuation_progress;
                break;
            case JungianArchetype::TRICKSTER:
                influence = reality_distortion_level;
                break;
            // ... handle other archetypes
        }
    }
}

void PersonalityProfile::simulate_growth(double delta) {
    // Natural stress reduction over time
    stress_level = Math::max(0.0f, stress_level - (float)delta * 0.1f);
    
    // Growth potential increases with self-awareness
    if (development.level <= 3) {
        growth_potential = Math::min(1.0f, growth_potential + (float)delta * 0.05f);
    }
    
    calculate_growth_direction();
    
    // Update psychological elements
    process_archetype_dynamics();
    update_symbolic_state();
    integrate_shadow_aspects();
    
    // Progress individuation
    if (dsm_traits.shadow_integration > 0.7f) {
        individuation_progress = Math::min(1.0f, individuation_progress + delta * 0.1f);
    }
    
    // Update beast/cosmic mechanics
    update_beast_state(delta);
    update_blood_corruption(delta);
    
    // Process cosmic influence
    if (core_traits.cosmic_insight.eyes_on_the_inside) {
        reality_distortion_level = Math::min(1.0f, reality_distortion_level + delta * 0.1f);
    }
}

void PersonalityProfile::calculate_growth_direction() {
    if (growth_potential > 0.8f && development.level > 1) {
        // Move towards growth direction
        int growth_point = static_cast<int>(primary_type);
        switch (growth_point) {
            case 1: growth_point = 7; break;
            case 2: growth_point = 4; break;
            case 3: growth_point = 6; break;
            case 4: growth_point = 1; break;
            case 5: growth_point = 8; break;
            case 6: growth_point = 9; break;
            case 7: growth_point = 5; break;
            case 8: growth_point = 2; break;
            case 9: growth_point = 3; break;
        }
        
        // Gradually improve development level
        development.level = Math::max(1, development.level - 1);
        update_development_level();
    }
}

void PersonalityProfile::handle_stress(const Dictionary& stressor) {
    float intensity = stressor["intensity"];
    stress_level = Math::clamp(stress_level + intensity, 0.0f, 1.0f);
    process_stress_response();
    
    // Reduce growth potential under stress
    growth_potential = Math::max(0.0f, growth_potential - intensity * 0.2f);
}

Dictionary PersonalityProfile::get_growth_opportunities() const {
    Dictionary opportunities;
    
    // Calculate potential growth areas based on current state
    opportunities["development_level"] = development.level;
    opportunities["growth_potential"] = growth_potential;
    
    Array potential_improvements;
    if (development.level > 1) {
        potential_improvements.push_back("self_awareness");
        potential_improvements.push_back("emotional_regulation");
    }
    if (stress_level > 0.5f) {
        potential_improvements.push_back("stress_management");
        potential_improvements.push_back("coping_mechanisms");
    }
    
    opportunities["areas"] = potential_improvements;
    return opportunities;
}

Dictionary PersonalityProfile::get_current_state() const {
    Dictionary state;
    state["enneagram_type"] = static_cast<int>(primary_type);
    state["development_level"] = development.level;
    state["stress_level"] = stress_level;
    state["growth_potential"] = growth_potential;
    
    Dictionary emotions;
    for (const auto& [emotion, intensity] : emotional_state) {
        emotions[emotion] = intensity;
    }
    state["emotional_state"] = emotions;
    
    state["behavioral_history"] = behavioral_history;
    return state;
}

Dictionary PersonalityProfile::to_dictionary() const {
    Dictionary data = get_current_state();
    
    // Add DSM traits
    Dictionary dsm;
    dsm["negative_affectivity"] = dsm_traits.negative_affectivity;
    dsm["detachment"] = dsm_traits.detachment;
    dsm["antagonism"] = dsm_traits.antagonism;
    dsm["disinhibition"] = dsm_traits.disinhibition;
    dsm["psychoticism"] = dsm_traits.psychoticism;
    data["dsm_traits"] = dsm;
    
    // Add core traits
    Array fears, desires, mechanisms;
    for (const auto& fear : core_traits.fears) fears.push_back(fear);
    for (const auto& desire : core_traits.desires) desires.push_back(desire);
    for (const auto& mechanism : core_traits.defense_mechanisms) mechanisms.push_back(mechanism);
    
    Dictionary core;
    core["fears"] = fears;
    core["desires"] = desires;
    core["defense_mechanisms"] = mechanisms;
    data["core_traits"] = core;
    
    return data;
}

void PersonalityProfile::from_dictionary(const Dictionary& data) {
    // Restore basic state
    primary_type = static_cast<EnneagramType>(static_cast<int>(data["enneagram_type"]));
    development.level = data["development_level"];
    stress_level = data["stress_level"];
    growth_potential = data["growth_potential"];
    
    // Restore emotional state
    Dictionary emotions = data["emotional_state"];
    Array keys = emotions.keys();
    for (int i = 0; i < keys.size(); i++) {
        String emotion = keys[i];
        emotional_state[emotion] = emotions[emotion];
    }
    
    // Restore behavioral history
    behavioral_history = data["behavioral_history"];
    
    // Restore DSM traits
    Dictionary dsm = data["dsm_traits"];
    dsm_traits.negative_affectivity = dsm["negative_affectivity"];
    dsm_traits.detachment = dsm["detachment"];
    dsm_traits.antagonism = dsm["antagonism"];
    dsm_traits.disinhibition = dsm["disinhibition"];
    dsm_traits.psychoticism = dsm["psychoticism"];
    
    // Restore core traits
    Dictionary core = data["core_traits"];
    Array fears = core["fears"];
    Array desires = core["desires"];
    Array mechanisms = core["defense_mechanisms"];
    
    core_traits.fears.clear();
    core_traits.desires.clear();
    core_traits.defense_mechanisms.clear();
    
    for (int i = 0; i < fears.size(); i++) core_traits.fears.push_back(fears[i]);
    for (int i = 0; i < desires.size(); i++) core_traits.desires.push_back(desires[i]);
    for (int i = 0; i < mechanisms.size(); i++) core_traits.defense_mechanisms.push_back(mechanisms[i]);
    
    update_development_level();
}

Dictionary PersonalityProfile::get_symbolic_state() const {
    Dictionary state;
    Array symbols;
    
    for (const auto& surreal_state : current_symbolic_state) {
        Dictionary symbol;
        symbol["symbol"] = surreal_state.symbol;
        symbol["distortion"] = surreal_state.distortion;
        symbol["archetype"] = surreal_state.archetype;
        symbol["metaphor"] = surreal_state.metaphor;
        symbols.push_back(symbol);
    }
    
    state["symbols"] = symbols;
    state["reality_distortion"] = reality_distortion_level;
    return state;
}

float PersonalityProfile::get_reality_distortion() const {
    return reality_distortion_level;
}

float PersonalityProfile::get_individuation_progress() const {
    return individuation_progress;
}

void PersonalityProfile::process_blood_resonance(float intensity) {
    core_traits.blood_resonance.beast_hood += intensity * 0.2f;
    blood_corruption_level = Math::min(1.0f, blood_corruption_level + intensity * 0.1f);
    
    // Process blood memories
    if (Math::randf() < intensity * 0.3f) {
        String memory = "blood_memory_" + String::num(core_traits.blood_echoes.size());
        core_traits.blood_echoes.push_back(memory);
        integrate_blood_memory(memory);
    }
    
    calculate_transformation_threshold();
}

void PersonalityProfile::update_beast_state(double delta) {
    BeastAspects& beast = core_traits.beast_aspects;
    BloodResonance& blood = core_traits.blood_resonance;
    
    // Update blood corruption
    update_blood_corruption(delta);
    
    // Check for state transitions
    switch (beast.current_state) {
        case BeastState::HUMAN:
            if (blood.beast_hood > transformation_threshold) {
                beast.current_state = BeastState::RESONANT;
                add_surreal_state("blood_echoes", 0.4f);
            }
            break;
            
        case BeastState::RESONANT:
            beast.transformation_progress += delta * blood.beast_hood * 0.1f;
            if (beast.transformation_progress > 0.7f) {
                beast.current_state = BeastState::TRANSFORMING;
                add_surreal_state("beast_vision", 0.8f);
            }
            break;
            
        case BeastState::TRANSFORMING:
            beast.transformation_progress += delta * blood.beast_hood * 0.2f;
            if (beast.transformation_progress >= 1.0f) {
                beast.current_state = BeastState::BEAST;
                trigger_beast_transformation(1.0f);
            }
            break;
            
        case BeastState::BEAST:
            if (core_traits.cosmic_insight.eyes_on_the_inside) {
                beast.current_state = BeastState::TRANSCENDENT;
                add_surreal_state("cosmic_awareness", 1.0f);
            }
            break;
            
        default:
            break;
    }
}

void PersonalityProfile::handle_cosmic_insight(const Dictionary& revelation) {
    CosmicInsight& insight = core_traits.cosmic_insight;
    
    // Process the revelation
    if (revelation.has("truth_level")) {
        float truth = revelation["truth_level"];
        insight.eldritch_truth += truth * 0.3f;
        
        // Increase fear with truth unless we have high insight
        if (insight.insight < 60.0f) {
            insight.fear_level += truth * 0.4f;
        }
    }
    
    // Check for eyes on the inside
    if (insight.eldritch_truth > 80.0f && !insight.eyes_on_the_inside) {
        insight.eyes_on_the_inside = true;
        cosmic_revelations.push_back(revelation);
        process_cosmic_revelation();
    }
    
    // Update cosmic visions
    Dictionary vision;
    vision["intensity"] = insight.eldritch_truth;
    vision["fear"] = insight.fear_level;
    insight.cosmic_visions = vision;
}

void PersonalityProfile::update_blood_corruption(double delta) {
    BloodResonance& blood = core_traits.blood_resonance;
    
    // Natural corruption progression
    blood_corruption_level += delta * blood.beast_hood * 0.05f;
    
    // Update blood resonance based on corruption
    blood.blood_corruption = blood_corruption_level;
    
    // Affect insight based on corruption
    if (blood_corruption_level > 0.7f) {
        blood.insight += delta * 0.1f;
    }
}

void PersonalityProfile::process_cosmic_revelation() {
    CosmicInsight& insight = core_traits.cosmic_insight;
    
    // Process each revelation's impact
    for (const Dictionary& revelation : cosmic_revelations) {
        if (revelation.has("cosmic_truth")) {
            String truth = revelation["cosmic_truth"];
            
            // Add unique beast traits based on cosmic understanding
            if (!core_traits.beast_aspects.beast_traits.contains(truth)) {
                core_traits.beast_aspects.beast_traits.push_back(truth);
            }
        }
    }
    
    // Update reality distortion based on cosmic insight
    reality_distortion_level = Math::max(reality_distortion_level, insight.eldritch_truth * 0.01f);
}

void PersonalityProfile::integrate_blood_memory(const String& memory) {
    Dictionary memory_data;
    memory_data["type"] = "blood_memory";
    memory_data["content"] = memory;
    
    // Add to behavioral history
    if (!behavioral_history.has("blood_memories")) {
        behavioral_history["blood_memories"] = Array();
    }
    Array memories = behavioral_history["blood_memories"];
    memories.push_back(memory_data);
    behavioral_history["blood_memories"] = memories;
    
    // Update beast traits based on memories
    if (core_traits.beast_aspects.beast_traits.size() < 5) {
        core_traits.beast_aspects.beast_traits.push_back(memory);
    }
}

void PersonalityProfile::calculate_transformation_threshold() {
    // Base threshold affected by various factors
    transformation_threshold = 0.7f;
    
    // Modify based on insight
    transformation_threshold -= core_traits.blood_resonance.insight * 0.002f;
    
    // Modify based on corruption
    transformation_threshold -= blood_corruption_level * 0.3f;
    
    // Resistance from humanity
    transformation_threshold += core_traits.humanity * 0.2f;
}

void PersonalityProfile::trigger_beast_transformation(float catalyst_strength) {
    BeastAspects& beast = core_traits.beast_aspects;
    
    // Update beast state
    beast.current_state = BeastState::BEAST;
    beast.transformation_progress = 1.0f;
    
    // Add primal instincts
    Dictionary instincts;
    instincts["aggression"] = 0.8f + catalyst_strength * 0.2f;
    instincts["hunger"] = 0.9f;
    instincts["territory"] = 0.7f;
    beast.primal_instincts = instincts;
    
    // Affect personality
    dsm_traits.antagonism = Math::min(1.0f, dsm_traits.antagonism + 0.4f);
    dsm_traits.disinhibition = Math::min(1.0f, dsm_traits.disinhibition + 0.5f);
    
    // Create transformation event
    Dictionary event;
    event["type"] = "transformation";
    event["catalyst_strength"] = catalyst_strength;
    process_event(event);
}

void PersonalityProfile::resist_transformation(float willpower) {
    core_traits.humanity = Math::min(1.0f, core_traits.humanity + willpower * 0.2f);
    core_traits.beast_resistance += willpower * 0.1f;
    
    // Reduce beast hood based on resistance
    core_traits.blood_resonance.beast_hood = Math::max(0.0f, 
        core_traits.blood_resonance.beast_hood - willpower * 0.3f);
}

BeastState PersonalityProfile::get_beast_state() const {
    return core_traits.beast_aspects.current_state;
}

float PersonalityProfile::get_insight_level() const {
    return core_traits.blood_resonance.insight;
}

Array PersonalityProfile::get_blood_memories() const {
    Array memories;
    for (const String& memory : core_traits.blood_echoes) {
        memories.push_back(memory);
    }
    return memories;
}

bool PersonalityProfile::has_eyes_on_the_inside() const {
    return core_traits.cosmic_insight.eyes_on_the_inside;
}

Dictionary PersonalityProfile::get_cosmic_state() const {
    Dictionary state;
    state["eldritch_truth"] = core_traits.cosmic_insight.eldritch_truth;
    state["fear_level"] = core_traits.cosmic_insight.fear_level;
    state["eyes_on_the_inside"] = core_traits.cosmic_insight.eyes_on_the_inside;
    state["cosmic_visions"] = core_traits.cosmic_insight.cosmic_visions;
    return state;
}

void PersonalityProfile::process_resonance(float intensity) {
    CoreTraits& core = core_traits;
    core.resonance_intensity += intensity * 0.2f;
    
    // Process psychological echoes
    if (Math::randf() < intensity * 0.3f) {
        String echo = "psych_echo_" + String::num(core.psychological_echoes.size());
        core.psychological_echoes.push_back(echo);
    }
    
    calculate_consciousness_threshold();
}

void PersonalityProfile::update_perception_layer(double delta) {
    CoreTraits& core = core_traits;
    
    // Update resonance based on current state
    switch (core.resonance_state) {
        case ResonanceState::GROUNDED:
            if (core.resonance_intensity > consciousness_threshold) {
                core.resonance_state = ResonanceState::RESONATING;
                add_surreal_state("consciousness_ripple", 0.4f);
            }
            break;
            
        case ResonanceState::RESONATING:
            core.consciousness_integration += delta * core.resonance_intensity * 0.1f;
            if (core.consciousness_integration > 0.7f) {
                core.resonance_state = ResonanceState::SYNCHRONIZED;
                add_surreal_state("shared_consciousness", 0.8f);
            }
            break;
            
        case ResonanceState::SYNCHRONIZED:
            core.consciousness_integration += delta * core.resonance_intensity * 0.2f;
            if (core.consciousness_integration >= 1.0f) {
                core.resonance_state = ResonanceState::TRANSCENDENT;
                trigger_reality_shift(1.0f);
            }
            break;
            
        case ResonanceState::TRANSCENDENT:
            if (core.inner_truth.reality_shaper) {
                core.resonance_state = ResonanceState::UNIFIED;
                add_surreal_state("collective_unity", 1.0f);
            }
            break;
            
        default:
            break;
    }
}

void PersonalityProfile::handle_inner_revelation(const Dictionary& revelation) {
    InnerTruth& truth = core_traits.inner_truth;
    
    // Process the revelation
    if (revelation.has("awareness_level")) {
        float awareness = revelation["awareness_level"];
        truth.self_awareness += awareness * 0.3f;
        
        // Increase collective connection with awareness
        if (truth.self_awareness > 60.0f) {
            truth.collective_connection += awareness * 0.2f;
        }
    }
    
    // Check for reality shaping ability
    if (truth.collective_connection > 80.0f && !truth.reality_shaper) {
        truth.reality_shaper = true;
        core_traits.inner_revelations.push_back(revelation);
    }
    
    // Update manifestations
    Dictionary manifestation;
    manifestation["awareness"] = truth.self_awareness;
    manifestation["connection"] = truth.collective_connection;
    truth.manifestations = manifestation;
}

float PersonalityProfile::calculate_consciousness_threshold() const {
    // Base threshold affected by various factors
    float threshold = 0.7f;
    
    // Modify based on self-awareness
    threshold -= core_traits.inner_truth.self_awareness * 0.002f;
    
    // Modify based on reality distortion
    threshold -= core_traits.reality_distortion * 0.3f;
    
    // Resistance from integration
    threshold += core_traits.consciousness_integration * 0.2f;
    
    return threshold;
}

void PersonalityProfile::trigger_reality_shift(float catalyst_strength) {
    CoreTraits& core = core_traits;
    
    // Update perception layer
    core.perception_layer = PerceptionLayer::TRANSFORMATIVE;
    core.reality_integration = 1.0f;
    
    // Add psychological manifestations
    Dictionary manifestations;
    manifestations["reality_influence"] = 0.8f + catalyst_strength * 0.2f;
    manifestations["collective_access"] = 0.9f;
    manifestations["pattern_recognition"] = 0.7f;
    core.inner_truth.manifestations = manifestations;
    
    // Affect personality dimensions
    dsm_traits.psychoticism = Math::min(1.0f, dsm_traits.psychoticism + 0.4f);
    dsm_traits.detachment = Math::min(1.0f, dsm_traits.detachment + 0.5f);
    
    // Create shift event
    Dictionary event;
    event["type"] = "reality_shift";
    event["catalyst_strength"] = catalyst_strength;
    process_event(event);
}

void PersonalityProfile::resist_dissolution(float willpower) {
    core_traits.consciousness_integration = Math::min(1.0f, 
        core_traits.consciousness_integration + willpower * 0.2f);
    core_traits.reality_integration += willpower * 0.1f;
    
    // Reduce resonance based on resistance
    core_traits.resonance_intensity = Math::max(0.0f, 
        core_traits.resonance_intensity - willpower * 0.3f);
}

ResonanceState PersonalityProfile::get_resonance_state() const {
    return core_traits.resonance_state;
}

float PersonalityProfile::get_consciousness_level() const {
    return core_traits.resonance_intensity;
}

Array PersonalityProfile::get_psychological_echoes() const {
    Array echoes;
    for (const String& echo : core_traits.psychological_echoes) {
        echoes.push_back(echo);
    }
    return echoes;
}

bool PersonalityProfile::has_reality_shaping() const {
    return core_traits.inner_truth.reality_shaper;
}

Dictionary PersonalityProfile::get_inner_state() const {
    Dictionary state;
    state["self_awareness"] = core_traits.inner_truth.self_awareness;
    state["collective_connection"] = core_traits.inner_truth.collective_connection;
    state["reality_shaper"] = core_traits.inner_truth.reality_shaper;
    state["manifestations"] = core_traits.inner_truth.manifestations;
    return state;
} 