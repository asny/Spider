#version 150

uniform mat4 MVMatrix;
uniform float time;
uniform vec3 eyePosition;

in vec3 position;
in vec3 normal;

void main()
{
    // Calculate position
    float t = 0.05 * time;
    gl_Position = MVMatrix * vec4(eyePosition + cos(t) * position + sin(t) * cross(position, normal), 1.);
}
