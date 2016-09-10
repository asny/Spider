#version 150

uniform mat4 MVMatrix;
uniform float time;

in vec3 position;
in vec3 normal;

void main()
{
    // Calculate position
    float t = 0.1 * time;
    gl_Position = MVMatrix * vec4(cos(t) * position + sin(t) * cross(position, normal), 1.);
}
