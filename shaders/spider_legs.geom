#version 150

layout (points) in;
layout (triangle_strip, max_vertices = 264) out;

uniform mat4 NMatrix;
uniform mat4 MVMatrix;
uniform mat4 PMatrix;

out vec3 pos;
out vec3 nor;

const float step_size = 0.33f;
const float radius = 0.05f;

vec3 up_direction;

void emit_vertex(vec3 position, vec3 normal)
{
    pos = position;
    nor = normal;
    gl_Position = PMatrix * vec4(pos, 1.);
    EmitVertex();
}

float func(float x)
{
    if(x < 0.33f)
    {
        return x;
    }
    if(x < 0.66f)
    {
        return 0.33f;
    }
    return 1.f - x;
}

vec3 compute_position(vec3 origin, vec3 foot, float parameter)
{
    vec3 vec = foot - origin;
    return origin + parameter * vec + func(parameter) * up_direction;
}

void emit_leg(vec3 origin, vec3 foot)
{
    for (float parameter = 0.f; parameter < 1.f-step_size; parameter += step_size)
    {
        float r = radius * (1.f - parameter);
        float next_r = radius * (1.f - parameter - step_size);
        vec3 center = compute_position(origin, foot, parameter);
        vec3 next_center = compute_position(origin, foot, parameter + step_size);
        
        vec3 axis = normalize(next_center - center);
        vec3 v = cross(axis, up_direction);
        for (float angle = 0.f; angle < 360.f; angle += 45.f)
        {
            vec3 n = v * cos(angle) + cross(axis, v) * sin(angle) + axis * dot(axis,v) * (1.f-cos(angle));
            emit_vertex(next_center + next_r * n, n);
            emit_vertex(center + r * n, n);
        }
        EndPrimitive();
    }
}

void main()
{
    up_direction = normalize((NMatrix * vec4(0., 1., 0., 1.)).xyz);
    vec3 spider_position = (MVMatrix * vec4(0., 0., 0., 1.)).xyz;
    vec3 foot = gl_in[0].gl_Position.xyz;
    
    emit_leg(spider_position + 0.07 * (foot - spider_position), foot);
}
