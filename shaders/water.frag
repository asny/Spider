#version 330

uniform samplerCube environmentMap;
uniform vec3 eyePosition;
uniform sampler2D depthMap;

in vec3 pos;
in vec3 nor;
in vec2 coords;

layout (location = 0) out vec4 color;

const float Eta = 1. / 1.5; // Ratio of indices of refraction
const float FresnelPower = 5.0;
const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));
const vec3 absorptionColor = vec3(0.,0.3,0.3);

float linearDepth(float depthSample)
{
    float zNear = 0.1; // camera z near
    float zFar = 100.0; // camera z far
    depthSample = 2.0 * depthSample - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
}

void main()
{
    vec3 incidentDir = normalize(pos - eyePosition.xyz);
    vec3 normal = normalize(nor);
    
    // Compute cosine to the incident angle
    float cosAngle = dot(normal, -incidentDir);
    
    // Compute fresnel approximation
    float fresnel = mix(F, 1.f, pow(1. - max(cosAngle, 0.), FresnelPower));
    
    // Reflection
    vec3 reflectDir = normalize(reflect(incidentDir, normal));
    vec3 reflectColor = texture(environmentMap, reflectDir).xyz;
    
    // Absorption
    float depth = linearDepth(texture(depthMap, gl_FragCoord.xy/vec2(1400., 700.)).x) - distance(eyePosition, pos);
    float absorption = 0.2 + 0.4 * depth;
    
    // Mix absorption, reflection and refraction
    color = vec4(mix(reflectColor, absorptionColor, absorption/(fresnel + absorption)), fresnel + absorption);
}
