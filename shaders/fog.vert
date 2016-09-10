#version 150

uniform mat4 MVMatrix;
uniform float time;

in vec3 position;
in vec3 normal;

void main()
{
    // Calculate position
    gl_Position = MVMatrix * vec4(cos(time) * position + sin(time) * cross(position, normal), 1.);
}
