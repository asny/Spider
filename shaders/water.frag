#version 330

uniform samplerCube environmentMap;
uniform vec3 eyePosition;
uniform sampler2D positionMap;
uniform sampler2D colorMap;

in vec3 pos;
in vec3 nor;
in vec2 coords;

layout (location = 0) out vec4 color;

const float Eta = 1. / 1.5; // Ratio of indices of refraction
const float FresnelPower = 5.0;
const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));

const vec3 scattering = vec3(0.2, 0.4, 0.2); // Scattering coefficient (due to particles in the water)
const vec3 absorption = vec3(0.4, 0.955, 0.99); // Absorption coefficient
const vec3 c = scattering * absorption;
const vec3 equilibriumColorAtInfinity = vec3(0., 0.1, 0.14); // Water color at "infinity"

void main()
{
    vec3 incidentDir = normalize(pos - eyePosition.xyz);
    vec3 normal = normalize(nor);
    vec2 screen_uv = gl_FragCoord.xy/vec2(1400., 700.) - 0.05 * normal.xz;
    vec3 backgroundColor = texture(colorMap, screen_uv).xyz;
    float depth = distance(eyePosition, texture(positionMap, screen_uv).xyz);
    
    // Compute cosine to the incident angle
    float cosAngle = dot(normal, -incidentDir);
    
    // Compute fresnel approximation
    float fresnel = mix(F, 1.f, pow(1. - max(cosAngle, 0.), FresnelPower));
    
    // Reflection
    vec3 reflectDir = normalize(reflect(incidentDir, normal));
    vec3 reflectColor = texture(environmentMap, reflectDir).xyz;
    
    // Refraction
    float waterDepth = depth - distance(eyePosition, pos);
    vec3 a = vec3(clamp( pow(c.r, waterDepth), 0., 1.), clamp( pow(c.g, waterDepth), 0., 1.), clamp( pow(c.b, waterDepth), 0., 1.));
    vec3 refractColor = a * 0.8 * backgroundColor + (1 - a) * equilibriumColorAtInfinity;
    
    // Mix refraction and reflection
    color = vec4(mix(refractColor, reflectColor, fresnel), 1.);
}
