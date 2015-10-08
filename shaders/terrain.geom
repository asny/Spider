#version 150

layout (triangles) in;
layout (triangle_strip, max_vertices = 9) out;

uniform mat4 PMatrix;

out vec3 pos;
out vec3 nor;

vec3 calc_normal(vec3 p1, vec3 p2, vec3 p3)
{
    return normalize(cross(p2 - p1, p3 - p1));
}

vec3 calc_subdivision_point(vec3 p1, vec3 p2, vec3 p3)
{
    vec3 normal = calc_normal(p1, p2, p3);
    return (p1 + p2 + p3) / 3.f + normal * 0.1f;
}

void emit_vertex(vec3 p)
{
    pos = p;
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
}

void emit_triangle(vec3 p1, vec3 p2, vec3 p3)
{
    nor = calc_normal(p1, p2, p3);
    emit_vertex(p1);
    emit_vertex(p2);
    emit_vertex(p3);
    
    EndPrimitive();
}

void subdivide(vec3 p1, vec3 p2, vec3 p3)
{
    vec3 center = calc_subdivision_point(p1, p2, p3);
    emit_triangle(center, p1, p2);
    emit_triangle(center, p2, p3);
    emit_triangle(center, p3, p1);
}

void main()
{
    vec3 p1 = gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[1].gl_Position.xyz;
    vec3 p3 = gl_in[2].gl_Position.xyz;
    
    subdivide(p1, p2, p3);
}
