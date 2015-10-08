#version 150

layout (triangles) in;
layout (triangle_strip, max_vertices = 9) out;

uniform mat4 PMatrix;

out vec3 pos;
out vec3 nor;

struct Triangle {
    vec3 p1;
    vec3 p2;
    vec3 p3;
};

vec3 calc_normal(Triangle triangle)
{
    return normalize(cross(triangle.p2 - triangle.p1, triangle.p3 - triangle.p1));
}

vec3 calc_subdivision_point(Triangle triangle)
{
    vec3 normal = calc_normal(triangle);
    return (triangle.p1 + triangle.p2 + triangle.p3) / 3.f + normal * 0.1f;
}

void emit_vertex(vec3 p)
{
    pos = p;
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
}

void emit_triangle(Triangle triangle)
{
    nor = calc_normal(triangle);
    emit_vertex(triangle.p1);
    emit_vertex(triangle.p2);
    emit_vertex(triangle.p3);
    
    EndPrimitive();
}

void main()
{
    vec3 p1 = gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[1].gl_Position.xyz;
    vec3 p3 = gl_in[2].gl_Position.xyz;
    
    const int depth = 1;
    Triangle[depth * 3 + 1] triangles;
    
    triangles[0] = Triangle(p1,p2,p3);
    
    for (int d = depth; d > 0; d--)
    {
        for (int i = 0; i < 1; i++)
        {
            Triangle triangle = triangles[i];
            vec3 center = calc_subdivision_point(triangle);
            triangles[1] = Triangle(center, triangle.p1, triangle.p2);
            triangles[2] = Triangle(center, triangle.p2, triangle.p3);
            triangles[3] = Triangle(center, triangle.p3, triangle.p1);
        }
    }
    
    for (int i = 0; i < triangles.length(); i++)
    {
        emit_triangle(triangles[i]);
    }
}
