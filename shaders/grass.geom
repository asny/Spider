#version 150

layout (lines) in;
layout (triangle_strip, max_vertices = 264) out;

uniform mat4 PMatrix;

out vec3 pos;
out vec3 nor;

const float step_size = 0.05f;

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

float func(float x)
{
    return sqrt(x);
}

vec3 func(vec3 origin, vec3 top, float parameter)
{
    vec3 vec = top - origin;
    return origin + vec3(parameter * vec.x, func(parameter) * vec.y, parameter * vec.z);
}

void emit_straw_half(vec3 origin1, vec3 origin2, vec3 top)
{
    for (float parameter = 0.f; parameter < 1.f - step_size; parameter += step_size)
    {
        vec3 p1 = func(origin1, top, parameter);
        vec3 p2 = func(origin2, top, parameter);
        vec3 p3 = func(origin1, top, parameter + step_size);
        vec3 p4 = func(origin2, top, parameter + step_size);
        
        vec3 n = calc_normal(Triangle(p1, p2, p3));
        emit_vertex(p1, n);
        emit_vertex(p2, n);
        emit_vertex(p3, n);
        emit_vertex(p4, n);
    }
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
    
    const float half_width = 0.05;
    vec3 corner = origin + half_width * leave_direction - half_width * bend_direction;
    emit_straw_half(corner, origin, top);
    corner = origin - half_width * leave_direction - half_width * bend_direction;
    emit_straw_half(origin, corner, top);
}
