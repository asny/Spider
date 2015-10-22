#version 150

uniform mat4 MVMatrix;

in vec3 end_point;

void main()
{
    // Calculate position
    gl_Position = MVMatrix * vec4(end_point, 1.);
}
