#version 330

uniform samplerCube environmentMap;
uniform vec3 eyePosition;
uniform float time;
uniform vec3 windDirection;
uniform vec4 ringCenterAndTime[32];
uniform int noEffects;
uniform float ringEffectTime;

uniform sampler2D positionMap;
uniform sampler2D colorMap;
uniform sampler2D maskSampler;
uniform vec2 screenSize;

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
    vec3 ring = vec3(0., 0., 0.);
    for (int i = 0; i < noEffects; i++)
    {
        vec3 center = ringCenterAndTime[i].xyz;
        float startTime = ringCenterAndTime[i].w;
        float dist = distance(pos, center);
        float timeSinceStart = time - startTime;
        float spread = 0.25 * timeSinceStart;
        if(startTime >= 0. && dist < spread)
        {
            float fadeFactor = 1. - smoothstep(0., ringEffectTime, timeSinceStart);
            float ringFactor = smoothstep(0., spread, dist) * sin(100.*(spread - dist));
            ring += fadeFactor * ringFactor * normalize(pos - center);
        }
    }
    
    vec3 normal = normalize(nor + ring);
    vec2 screen_uv = gl_FragCoord.xy/screenSize - 0.05 * normal.xz; // Shift the water bottom.
    vec3 bottomPos = texture(positionMap, screen_uv).xyz;
    float distanceToWater = distance(eyePosition, pos);
    float distanceToBottom = distance(eyePosition, bottomPos);
    if(distanceToWater > distanceToBottom)
    {
        // Do not shift something that is in front of the water.
        screen_uv = gl_FragCoord.xy/screenSize;
        bottomPos = texture(positionMap, screen_uv).xyz;
        distanceToWater = distance(eyePosition, pos);
        distanceToBottom = distance(eyePosition, bottomPos);
    }
    vec3 incidentDir = (pos - eyePosition.xyz)/distanceToWater;
    vec3 backgroundColor = texture(colorMap, screen_uv).xyz;
    
    // Compute cosine to the incident angle
    float cosAngle = dot(normal, -incidentDir);
    
    // Compute fresnel approximation
    float fresnel = mix(F, 1.f, pow(1. - max(cosAngle, 0.), FresnelPower));
    
    // Reflection
    vec3 reflectDir = normalize(reflect(incidentDir, normal));
    vec3 reflectColor = mix(texture(environmentMap, reflectDir).xyz, vec3(1., 1., 1.), 0.5);
    
    // Refraction
    float viewWaterDepth = distanceToBottom - distanceToWater;
    vec3 a = vec3(clamp( pow(c.r, viewWaterDepth), 0., 1.), clamp( pow(c.g, viewWaterDepth), 0., 1.), clamp( pow(c.b, viewWaterDepth), 0., 1.));
    vec3 refractColor = a * backgroundColor + (1 - a) * equilibriumColorAtInfinity;
    
    // Mix refraction and reflection
    vec3 col = mix(refractColor, reflectColor, fresnel);
    
    // Foam
    float waterDepth = pos.y-bottomPos.y;
    const float minFoamDepth = 0.;
    const float peakFoamDepth = 0.02;
    const float maxFoamDepth = 0.1;
    if(waterDepth > minFoamDepth && waterDepth < maxFoamDepth)
    {
        float foam;
        if(waterDepth < peakFoamDepth)
        {
            foam = smoothstep(minFoamDepth, peakFoamDepth, waterDepth);
        }
        else {
            foam = 1. - ((waterDepth - peakFoamDepth) / (maxFoamDepth - peakFoamDepth));
        }
        
        // Sample the mask
        vec2 scaledUV = coords * 20.;
        float edgePatternScroll = 0.2;
        float channelA = texture(maskSampler, scaledUV - vec2(edgePatternScroll, cos(coords.x))).r;
        float channelB = texture(maskSampler, scaledUV * 0.5 + vec2(sin(coords.y), edgePatternScroll)).b;
        
        // Modify it to our liking
        float mask = (channelA + channelB) * 0.95;
        mask = pow(mask, 2);
        mask = clamp(mask, 0, 1);
        
        foam = clamp(foam - mask, 0, 1);
        col = mix(col, vec3(0.8,0.8,0.8), foam);
    }
    
    color = vec4(col, 1.);
}
