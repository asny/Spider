#version 150

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 PMatrix;
uniform float radius;

out vec3 center;
out vec3 pos;

void main()
{
    center = gl_in[0].gl_Position.xyz;
    
    pos = center + radius * vec3(-3., -1., 0.);
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
    
    pos = center + radius * vec3(3., -1., 0.);
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
    
    pos = center + radius * vec3(0., 2., 0.);
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
}
