#version 150

layout (triangles) in;
layout (triangle_strip, max_vertices = 11) out;

uniform mat4 PMatrix;

out vec3 pos;
out vec3 nor;

vec3 calc_normal(vec3 p1, vec3 p2, vec3 p3)
{
    return normalize(cross(p2 - p1, p3 - p1));
}

void emit_vertex(vec3 p)
{
    pos = p;
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
}

void subdivide(vec3 p1, vec3 p2, vec3 p3)
{
    vec3 normal = calc_normal(p1, p2, p3);
    vec3 center = (p1 + p2 + p3) / 3.f + normal * 0.1f;
    
    nor = calc_normal(center, p1, p2);
    emit_vertex(center);
    emit_vertex(p1);
    emit_vertex(p2);
    emit_vertex(center);

    nor = calc_normal(center, p2, p3);
    emit_vertex(center);
    emit_vertex(p2);
    emit_vertex(p3);
    emit_vertex(center);
    
    nor = calc_normal(center, p3, p1);
    emit_vertex(center);
    emit_vertex(p3);
    emit_vertex(p1);
}

void main()
{
    vec3 p1 = gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[1].gl_Position.xyz;
    vec3 p3 = gl_in[2].gl_Position.xyz;
    
    subdivide(p1, p2, p3);
    
    EndPrimitive();
}
