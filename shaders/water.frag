#version 330

uniform samplerCube environmentMap;
uniform vec3 eyePosition;
uniform sampler2D noiseTexture;

in vec3 pos;
in vec3 nor;
in vec2 coords;

layout (location = 0) out vec4 color;

const float Eta = 1. / 1.5; // Ratio of indices of refraction
const float FresnelPower = 5.0;
const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));
const vec4 waterColor = vec4(0.1,0.3,0.4, 0.5);

void main()
{
    vec3 incidentDir = normalize(pos - eyePosition.xyz);
    
    // Compute normal
    float noise = texture(noiseTexture, coords).x - 0.5;
    vec3 normal = normalize(nor + 0.5 * vec3(noise, 0., noise));
    
    // Compute cosine to the incident angle
    float cosAngle = dot(normal, -incidentDir);
    
    // Compute fresnel approximation
    float fresnel = mix(F, 1.f, pow(1. - max(cosAngle, 0.), FresnelPower));
    
    // Reflection
    vec3 reflectDir = normalize(reflect(incidentDir, normal));
    vec3 reflectColor = texture(environmentMap, reflectDir).xyz;
    
    // Mix water color, reflection and refraction (absorption and bottom texture)
    float opacity = mix(fresnel, 1.f, waterColor.w);
    color = vec4(mix(reflectColor, waterColor.xyz, waterColor.w/opacity), opacity);
}
