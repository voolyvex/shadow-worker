#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform float time;
uniform float distortionStrength;
uniform vec2 distortionCenter;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    // Calculate distance from distortion center
    vec2 pos = vertexPosition.xy;
    vec2 dir = pos - distortionCenter;
    float dist = length(dir);
    
    // Apply sinusoidal distortion based on distance and time
    float distortionFactor = sin(dist * 3.0 - time * 2.0) * distortionStrength;
    vec2 offset = normalize(dir) * distortionFactor;
    
    // Apply distortion to position
    vec3 distortedPosition = vec3(pos + offset, vertexPosition.z);
    
    // Calculate final vertex position
    gl_Position = mvp * vec4(distortedPosition, 1.0);
    
    // Pass vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
} 