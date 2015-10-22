#version 150

layout (lines) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 PMatrix;

out vec3 pos;
out vec3 nor;

void main()
{
    vec3 origin = gl_in[0].gl_Position.xyz;
    vec3 top = gl_in[1].gl_Position.xyz;
    vec3 extra_pos = origin + vec3(0.5,0.,0.);
    
    nor = normalize(cross(extra_pos - origin, top - origin));
    
    pos = origin;
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
    
    pos = extra_pos;
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
    
    pos = top;
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
    
    EndPrimitive();
}
