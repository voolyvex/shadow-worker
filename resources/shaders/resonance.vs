#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform float time;
uniform float resonanceStrength;
uniform vec2 resonanceCenter;
uniform float resonanceFrequency;
uniform float resonancePhase;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out float resonanceIntensity;

void main() {
    // Calculate distance from resonance center
    vec2 pos = vertexPosition.xy;
    vec2 dir = pos - resonanceCenter;
    float dist = length(dir);
    
    // Calculate resonance wave
    float wave = sin(dist * resonanceFrequency - time * 2.0 + resonancePhase);
    float amplitude = exp(-dist * 0.5) * resonanceStrength;
    
    // Apply resonance displacement
    vec2 offset = normalize(dir) * wave * amplitude;
    vec3 resonatedPosition = vec3(pos + offset, vertexPosition.z);
    
    // Calculate resonance intensity for fragment shader
    resonanceIntensity = amplitude * (wave * 0.5 + 0.5);
    
    // Calculate final vertex position
    gl_Position = mvp * vec4(resonatedPosition, 1.0);
    
    // Pass vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
} 