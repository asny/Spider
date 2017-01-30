#version 330

uniform samplerCube texture0;
uniform vec3 eyePosition;
uniform float time;

in vec3 pos;
in vec3 nor;

out vec4 fragColour;

const float Eta = 1. / 1.5; // Ratio of indices of refraction
const float FresnelPower = 5.0;
const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));

void main()
{
    vec3 normal = normalize(nor);
    vec3 incidentDir = normalize(pos - eyePosition.xyz);
    
    // Compute cosinus to the incident angle
    float cosAngle = dot(normal, -incidentDir);
    
    // Compute fresnel approximation
    float fresnel = mix(F, 1.f, pow(1. - max(cosAngle, 0.), FresnelPower));
    
    // Reflection
    vec3 reflectDir = normalize(reflect(incidentDir, normal));
    vec3 reflectColor = texture(texture0, reflectDir).xyz;
    
    // absorbtion
    vec3 absprbtionColor = vec3(0.3,0.4,0.5);
    float absorbtion = (1.f - fresnel) * 0.5f;
    
    // Mix reflection and absprbtion
    fragColour = vec4(mix(absprbtionColor, reflectColor, fresnel/(fresnel + absorbtion)), fresnel + absorbtion);
}
