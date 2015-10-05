#version 150

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform mat4 NormalMatrix;

in vec3 position;
in vec3 vector;

out vec3 pos;
out vec3 nor;

void main()
{
    
    pos = mat3(MVMatrix) * position;
    nor = mat3(NormalMatrix) * vector;
    // Calculate position
    gl_Position = MVPMatrix * vec4(position.xyz, 1.);
}