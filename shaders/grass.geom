#version 150

layout (lines) in;
layout (triangle_strip, max_vertices = 6) out;

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

void emit_vertex(vec3 position, vec3 normal)
{
    pos = position;
    nor = normal;
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
    vec3 origin = gl_in[0].gl_Position.xyz;
    vec3 top = gl_in[1].gl_Position.xyz;
    
    const vec3 up_direction = vec3(0., 1., 0.);
    vec3 straw_direction = normalize(top-origin);
    vec3 leave_direction = cross(up_direction, straw_direction);
    vec3 bend_direction = cross(leave_direction, up_direction);
    if(dot(bend_direction, straw_direction) < 0.)
    {
        bend_direction = -bend_direction;
    }
    
    emit_triangle(Triangle(origin, top, origin + 0.1 * leave_direction - 0.01 * bend_direction));
    emit_triangle(Triangle(origin, top, origin - 0.1 * leave_direction - 0.01 * bend_direction));
}
