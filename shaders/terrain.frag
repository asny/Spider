#version 330

uniform sampler2D groundTexture;
uniform sampler2D lakeTexture;
uniform sampler2D noiseTexture;
uniform float time;

in vec2 coords;
in vec3 nor;
in vec3 pos;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

void main()
{
    position = vec4(pos, 1.0);
    if(pos.y > 0.)
    {
        color = texture(groundTexture, coords);
    }
    else {
        float noise = texture(noiseTexture, coords).x - 0.5;
        float wave = sin(mod(time * (1.f + noise), 6.28));
        vec2 uv = coords
            - 0.1 * vec2(wave, wave)
            - 0.5 * vec2(noise, noise);
        vec3 bottomColor = texture(lakeTexture, uv).xyz;
        
        // absorption
        vec3 absorptionColor = vec3(0.,0.3,0.3);
        float absorption = min(max(0.5f + abs(pos.y + 0.25f), 0.f), 1.f);
        color = vec4(mix(bottomColor, absorptionColor, absorption), 1.);
    }
    normal = vec4(nor, 1.0);
}
