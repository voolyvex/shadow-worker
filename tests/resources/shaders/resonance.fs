#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in float resonanceIntensity;

// Input uniform values
uniform sampler2D texture0;
uniform float time;
uniform vec4 resonanceColor;
uniform float resonanceStrength;
uniform float psycheLevel;

// Output fragment color
out vec4 finalColor;

// Helper function for creating psychedelic patterns
vec3 getPsychePattern(vec2 uv, float t) {
    vec2 p = uv * 2.0 - 1.0;
    float a = atan(p.y, p.x);
    float r = length(p);
    float v = sin(r * 10.0 - t * 2.0);
    
    // Create color pattern based on angle and radius
    vec3 col;
    col.r = sin(a * 3.0 + t + v) * 0.5 + 0.5;
    col.g = sin(a * 4.0 + t * 1.5 + v) * 0.5 + 0.5;
    col.b = sin(a * 5.0 + t * 2.0 + v) * 0.5 + 0.5;
    
    return col;
}

void main() {
    // Sample base texture
    vec4 texColor = texture(texture0, fragTexCoord);
    
    // Create psychedelic effect based on resonance
    vec3 psychePattern = getPsychePattern(fragTexCoord, time);
    vec4 psycheColor = vec4(psychePattern * resonanceColor.rgb, resonanceColor.a);
    
    // Calculate resonance glow
    float glow = sin(time * 3.0) * 0.5 + 0.5;
    vec4 glowColor = resonanceColor * glow * resonanceIntensity;
    
    // Mix effects based on psyche level and resonance intensity
    float mixFactor = resonanceIntensity * psycheLevel;
    vec4 mixedColor = mix(texColor, psycheColor, mixFactor);
    
    // Add glow and apply resonance strength
    finalColor = mixedColor + glowColor * resonanceStrength;
    finalColor *= fragColor;
    
    // Add subtle pulsing transparency at high resonance
    if (resonanceIntensity > 0.8) {
        float pulse = sin(time * 5.0) * 0.2 + 0.8;
        finalColor.a *= pulse;
    }
} 