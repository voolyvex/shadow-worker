shader_type canvas_item;

// Global parameters
uniform float global_distortion : hint_range(0.0, 1.0) = 0.0;
uniform vec2 screen_resolution = vec2(1920.0, 1080.0);
uniform float time_elapsed;

// Effect-specific parameters
uniform float ripple_intensity : hint_range(0.0, 1.0) = 0.0;
uniform float ripple_speed : hint_range(0.0, 5.0) = 2.0;
uniform float ripple_frequency : hint_range(0.0, 50.0) = 10.0;

uniform float fragment_size : hint_range(0.0, 1.0) = 0.0;
uniform float fragment_rotation : hint_range(0.0, 360.0) = 0.0;

uniform float merge_factor : hint_range(0.0, 1.0) = 0.0;
uniform sampler2D merge_texture;

uniform float dissolve_progress : hint_range(0.0, 1.0) = 0.0;
uniform float dissolve_noise_scale = 30.0;

uniform float crystallize_intensity : hint_range(0.0, 1.0) = 0.0;
uniform float refraction_strength : hint_range(1.0, 2.0) = 1.0;

// Utility functions
float random(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec2 rotate_uv(vec2 uv, float angle, vec2 pivot) {
    float sin_a = sin(angle);
    float cos_a = cos(angle);
    uv -= pivot;
    return vec2(
        uv.x * cos_a - uv.y * sin_a,
        uv.x * sin_a + uv.y * cos_a
    ) + pivot;
}

// Effect implementations
vec2 apply_ripple(vec2 uv) {
    if (ripple_intensity <= 0.0) return uv;
    
    float dist = length(uv - 0.5);
    float angle = ripple_frequency * dist - ripple_speed * time_elapsed;
    float offset = ripple_intensity * 0.01 * sin(angle);
    
    return uv + vec2(offset);
}

vec2 apply_fragmentation(vec2 uv) {
    if (fragment_size <= 0.0) return uv;
    
    float grid_size = max(0.01, 1.0 - fragment_size);
    vec2 grid_uv = floor(uv / grid_size) * grid_size;
    float angle = fragment_rotation * random(grid_uv);
    
    return rotate_uv(uv, angle, grid_uv + grid_size * 0.5);
}

vec4 apply_merge(vec4 color, vec2 uv) {
    if (merge_factor <= 0.0) return color;
    
    vec4 merge_color = texture(merge_texture, uv);
    return mix(color, merge_color, merge_factor);
}

vec4 apply_dissolve(vec4 color, vec2 uv) {
    if (dissolve_progress <= 0.0) return color;
    
    float noise = random(uv * dissolve_noise_scale);
    float alpha = step(dissolve_progress, noise);
    
    return vec4(color.rgb, color.a * alpha);
}

vec4 apply_crystallize(vec4 color, vec2 uv) {
    if (crystallize_intensity <= 0.0) return color;
    
    // Create a crystalline pattern
    vec2 crystal_uv = uv * 10.0;
    vec2 cell = floor(crystal_uv);
    vec2 cell_uv = fract(crystal_uv);
    
    float cell_random = random(cell);
    float edge_factor = step(0.1, min(cell_uv.x, min(cell_uv.y, min(1.0-cell_uv.x, 1.0-cell_uv.y))));
    
    // Apply refraction
    vec2 refract_offset = (vec2(cell_random) - 0.5) * 0.1 * crystallize_intensity;
    vec4 refracted = texture(TEXTURE, uv + refract_offset);
    
    // Enhance contrast and brightness
    vec4 crystallized = mix(
        color,
        vec4(refracted.rgb * (1.0 + crystallize_intensity * 0.5), color.a),
        edge_factor
    );
    
    return crystallized;
}

void fragment() {
    // Get base UV coordinates
    vec2 uv = UV;
    
    // Apply progressive distortion effects
    uv = apply_ripple(uv);
    uv = apply_fragmentation(uv);
    
    // Sample the texture with distorted UVs
    vec4 color = texture(TEXTURE, uv);
    
    // Apply post-processing effects
    color = apply_merge(color, uv);
    color = apply_dissolve(color, uv);
    color = apply_crystallize(color, uv);
    
    // Apply global distortion intensity
    color.rgb = mix(color.rgb, color.rgb * (1.0 + global_distortion * 0.2), global_distortion);
    
    COLOR = color;
} 