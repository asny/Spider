#version 330

uniform sampler2D groundTexture;
uniform sampler2D lakeTexture;
uniform sampler2D noiseTexture;
uniform float time;
uniform vec3 windDirection;

in vec2 coords;
in vec3 nor;
in vec3 pos;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

void main()
{
    position = vec4(pos, 1.0);
    float noise = texture(noiseTexture, coords).x;
    if(pos.y > 0.)
    {
        vec3 sand_color = vec3(0.5, 0.5, 0.4) + 0.5 * vec3(noise, noise, noise);
        vec3 ground_color = texture(groundTexture, coords).xyz;
        color = vec4(mix(sand_color, ground_color, clamp(5. * pos.y, 0., 1.)), 1.);
    }
    else {
        float wave = sin(mod(2. * (pos.x * windDirection.x + pos.z * windDirection.z + time), 6.28));
        vec2 uv = coords
            - 0.01 * vec2(noise - 0.5, noise - 0.5)
            - 0.005 * windDirection.xz * wave;
        vec3 bottomColor = texture(lakeTexture, uv).xyz;
        
        // absorption
        vec3 absorptionColor = vec3(0.,0.3,0.3);
        float absorption = min(max(0.5f + abs(pos.y + 0.25f), 0.f), 1.f);
        color = vec4(mix(bottomColor, absorptionColor, absorption), 1.);
    }
    normal = vec4(nor, 1.0);
}
