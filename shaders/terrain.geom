#version 150

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

uniform mat4 PMatrix;

in vec3 n[1];

out vec3 pos;
out vec3 nor;

void main()
{
    vec3 p = gl_in[0].gl_Position.xyz;
    vec3 E = normalize(-p);
    vec3 V = normalize(n[0]);
    vec3 T = normalize(cross(E, V));
    vec3 D = 0.1*E;
    
    float w = 0.005*length(n[0]);
    
    pos = p + w * T + D;
    gl_Position = PMatrix * vec4(pos, 1.);
    nor = n[0];
    EmitVertex();
    
    pos = p - w * T + D;
    gl_Position = PMatrix * vec4(pos, 1.);
    nor = n[0];
    EmitVertex();
    
    pos = p - w * T + n[0] + D;
    gl_Position = PMatrix * vec4(pos, 1.);
    nor = n[0];
    EmitVertex();
    
    EndPrimitive();
    
    pos = p - w * T + n[0] + D;
    gl_Position = PMatrix * vec4(pos, 1.);
    nor = n[0];
    EmitVertex();
    
    pos = p + w * T + n[0] + D;
    gl_Position = PMatrix * vec4(pos, 1.);
    nor = n[0];
    EmitVertex();
    
    pos = p + w * T + D;
    gl_Position = PMatrix * vec4(pos, 1.);
    nor = n[0];
    EmitVertex();
    
    EndPrimitive();
}