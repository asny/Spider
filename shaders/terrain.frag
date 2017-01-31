#version 330

uniform sampler2D groundTexture;
uniform sampler2D lakeTexture;
uniform float time;

in vec2 coords;
in vec3 nor;
in vec3 pos;

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 color;
layout (location = 2) out vec4 normal;

void main()
{
    position = vec4(pos, 1.0);
    if(pos.y > 0.)
    {
        color = texture(groundTexture, coords);
    }
    else {
        vec2 uv = coords - 0.01 * vec2(cos(pos.x + time), sin(pos.z + 2. * time));
        vec3 bottomColor = texture(lakeTexture, uv).xyz;
        
        // absorption
        vec3 absorptionColor = vec3(0.2,0.3,0.4);
        float absorption = min(max(5.f * abs(pos.y + 0.2f), 0.f), 1.f);
        color = vec4(mix(bottomColor, absorptionColor, absorption), 1.);
    }
    normal = vec4(nor, 1.0);
}
