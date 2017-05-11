#version 330

in vec2 dens;
in vec3 nor;
in vec3 pos;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

void main()
{
    if(dens.x > 0 && dens.x < 1.)
    {
        position = vec4(pos, 1.0);
        normal = vec4(nor, 1.0);
        color = vec4(dens.x, 0., 0., 1.);
    }
    else {
        discard;
    }
}
