#version 150

uniform mat4 MVMatrix;
uniform mat4 NormalMatrix;

in vec3 position;
in vec3 vector;

out vec3 n;

void main()
{
    n = mat3(NormalMatrix) * vector;
    
    // Calculate position
    gl_Position = MVMatrix * vec4(position.xyz, 1.);
}
