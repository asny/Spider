#version 330

uniform sampler2D groundTexture;
uniform sampler2D lakeTexture;

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
        color = texture(lakeTexture, coords);
    }
    normal = vec4(nor, 1.0);
}
