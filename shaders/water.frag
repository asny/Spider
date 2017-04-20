#version 330

uniform samplerCube texture0;
uniform sampler2D noiseTexture;
uniform vec3 eyePosition;
uniform float time;
uniform vec3 windDirection;

in vec3 pos;
in vec3 nor;
in vec2 coords;

layout (location = 0) out vec4 color;

const float Eta = 1. / 1.5; // Ratio of indices of refraction
const float FresnelPower = 5.0;
const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));

void main()
{
    vec3 normal = normalize(nor);
    vec3 incidentDir = normalize(pos - eyePosition.xyz);
    
    // Perturb normal
    float noise = texture(noiseTexture, coords).x - 0.5;
    float wave = sin(mod(2. * (pos.x * windDirection.x + pos.z * windDirection.z + time), 6.28));
    normal = normalize(normal +
                       + 0.2 * vec3(noise, 0., noise)
                       + 0.1 * windDirection * wave);
    
    // Compute cosinus to the incident angle
    float cosAngle = dot(normal, -incidentDir);
    
    // Compute fresnel approximation
    float fresnel = mix(F, 1.f, pow(1. - max(cosAngle, 0.), FresnelPower));
    
    // Reflection
    vec3 reflectDir = normalize(reflect(incidentDir, normal));
    vec3 reflectColor = texture(texture0, reflectDir).xyz;
    
    // Mix reflection and absprbtion
    color = vec4(reflectColor, fresnel);
}
