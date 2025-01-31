#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform float time;
uniform float distortionStrength;
uniform vec2 distortionCenter;
uniform vec4 distortionColor;

// Output fragment color
out vec4 finalColor;

void main() {
    // Calculate distance from distortion center in UV space
    vec2 dir = fragTexCoord - distortionCenter;
    float dist = length(dir);
    
    // Create ripple effect
    float ripple = sin(dist * 20.0 - time * 3.0) * 0.5 + 0.5;
    
    // Calculate UV distortion
    float distortionAmount = ripple * distortionStrength * (1.0 - dist);
    vec2 distortedUV = fragTexCoord + normalize(dir) * distortionAmount;
    
    // Sample texture with distorted UVs
    vec4 texColor = texture(texture0, distortedUV);
    
    // Add color tint based on distortion
    vec4 tintedColor = mix(texColor, distortionColor, distortionAmount * 0.5);
    
    // Add edge glow
    float edgeGlow = smoothstep(0.4, 0.5, dist) * (1.0 - smoothstep(0.5, 0.6, dist));
    vec4 glowColor = distortionColor * edgeGlow * distortionStrength;
    
    // Combine effects
    finalColor = tintedColor + glowColor;
    finalColor *= fragColor;
} 