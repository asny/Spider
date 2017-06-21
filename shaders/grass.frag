#version 330

uniform vec3 materialColor;

in vec3 pos;
in vec3 nor;
in vec2 coords;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

void main(void)
{
    // Calculate colour
    float shadow = 0.5 + 0.5 * smoothstep(0., 0.5, abs(0.5 - coords.x));
    color = vec4(shadow * materialColor, 1.);
    position = vec4(pos, 1.0);
    normal = vec4(nor, 1.0);
}
