#version 330

uniform samplerCube texture0;
uniform vec3 eyePosition;
uniform float time;

in vec3 pos;
in vec3 nor;

out vec4 fragColour;

const float Eta = 1. / 1.5; // Ratio of indices of refraction
const float cosCriticalAngle = sqrt(1.f - 1.f/(Eta * Eta)); // Cosinus to the critical angle where there is total internal reflection
const float FresnelPower = 5.0;
const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));

void main()
{
    vec3 normal = normalize(nor);
    vec3 incidentDir = normalize(pos - eyePosition.xyz);
    
    // Perturb normal
//    float noise = texture(noiseTexture, noiseUV).x - 0.5;
//    normal = normalize(normal
//                       + 0.5 * vec3(noise, 0., noise)
//                       + 0.05 * vec3(cos(worldPos.x + 4. * time), 0., sin(worldPos.z + 2. * time)));
    
    // Reflection
    vec3 reflectDir = normalize(reflect(incidentDir, normal));
//    vec4 reflectColor = texture(tex, normalize(reflectDir - vec3(0., 1. - abs(reflectDir.y), 0.)));// Adjust so we only look up in the bottom texture
    vec4 reflectColor = texture(texture0, reflectDir);
    
    // Compute cosinus to the incident angle
    float cosAngle = dot(normal, -incidentDir);
    
    // Refraction
    vec3 refractDir = normalize(refract(incidentDir, normal, Eta));
    vec4 refractColor = texture(texture0, refractDir);
    
    // Compute fresnel approximation
    float fresnel = pow(1. - max((cosCriticalAngle - cosAngle) / (cosCriticalAngle - 1.f), 0.), FresnelPower);
    
    // Mix reflection and refraction
    fragColour = mix(refractColor, reflectColor, fresnel);
}
