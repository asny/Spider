#version 150

layout (triangles) in;
layout (triangle_strip, max_vertices = 256) out;

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

void emit_vertex(vec3 p, vec3 n)
{
    pos = p;
    nor = n;
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
}

void emit_triangle(Triangle triangle)
{
    vec3 n = calc_normal(triangle);
    emit_vertex(triangle.p1, n);
    emit_vertex(triangle.p2, n);
    emit_vertex(triangle.p3, n);
    
    EndPrimitive();
}

void main()
{
    const int depth = 2;
    const int no_triangles = int(pow(3, depth));
    
    // Add first triangle
    Triangle[no_triangles] triangles;
    triangles[0] = Triangle(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);
    
    int end_index = 1;
    for (int d = depth; d > 0; d--)
    {
        int end = end_index;
        for (int i = 0; i < end; i++)
        {
            Triangle triangle = triangles[i];
            vec3 center = calc_subdivision_point(triangle);
            
            triangles[i] = Triangle(center, triangle.p1, triangle.p2);
            
            triangles[end_index] = Triangle(center, triangle.p2, triangle.p3);
            end_index++;
            
            triangles[end_index] = Triangle(center, triangle.p3, triangle.p1);
            end_index++;
        }
    }
    
    for (int i = 0; i < triangles.length(); i++)
    {
        emit_triangle(triangles[i]);
    }
}
