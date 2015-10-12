#version 150

uniform mat4 MVMatrix;
uniform mat4 PMatrix;
uniform mat4 NormalMatrix;

in vec3 position;
in vec3 normal;

out vec3 pos;
out vec3 nor;

void main()
{
    pos = mat3(MVMatrix) * position;
    nor = mat3(NormalMatrix) * normal;
    
    // Calculate position
    gl_Position = PMatrix * MVMatrix * vec4(position.xyz, 1.);
}
