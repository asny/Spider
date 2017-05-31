#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 VPMatrix;

in VertexData {
    vec2 coords;
} VertexIn[];

out vec3 pos;
out vec3 nor;
out vec2 coords;

void main()
{
    vec3 p1 = gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[1].gl_Position.xyz;
    vec3 p3 = gl_in[2].gl_Position.xyz;
    
    nor = normalize(cross(p2 - p1, p3 - p1));
    
    pos = p1;
    coords = VertexIn[0].coords;
    gl_Position = VPMatrix * vec4(pos, 1.);
    EmitVertex();
    
    pos = p2;
    coords = VertexIn[1].coords;
    gl_Position = VPMatrix * vec4(pos, 1.);
    EmitVertex();
    
    pos = p3;
    coords = VertexIn[2].coords;
    gl_Position = VPMatrix * vec4(pos, 1.);
    EmitVertex();
    
    EndPrimitive();
}
