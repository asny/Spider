#version 150

uniform mat4 MVMatrix;
uniform mat4 NormalMatrix;

in vec3 position;

void main()
{
    // Calculate position
    gl_Position = MVMatrix * vec4(position.xyz, 1.);
}
