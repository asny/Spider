#version 150

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 PMatrix;

out vec3 center;
out vec3 pos;

void main()
{
    center = gl_in[0].gl_Position.xyz;
    
    pos = center + vec3(-1.5, -0.5, 0.);
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
    
    pos = center + vec3(1.5, -0.5, 0.);
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
    
    pos = center + vec3(0., 1., 0.);
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
}
