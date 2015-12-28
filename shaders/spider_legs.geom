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

void emit_tube(vec3 start, vec3 end, float start_radius, float end_radius)
{
    vec3 axis_direction = normalize(end - start);
    vec3 out_direction = cross(axis_direction, up_direction);
    
    for (float angle = 0.f; angle < 360.f; angle += 45.f)
    {
        vec3 n = out_direction * cos(angle) + cross(axis_direction, out_direction) * sin(angle) + axis_direction * dot(axis_direction, out_direction) * (1.f-cos(angle));
        emit_vertex(end + end_radius * n, n);
        emit_vertex(start + start_radius * n, n);
    }
    EndPrimitive();
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

void emit_joint(vec3 start, vec3 joint, vec3 end, float radius)
{
    for (float t = -0.1f; t < 0.1f; t += 0.1f)
    {
        emit_tube(joint + t * (start - joint), joint + t * (end - joint), radius, radius);
    }
}

void emit_leg(vec3 origin, vec3 foot)
{
    for (float parameter = 0.f; parameter < 1.f-step_size; parameter += step_size)
    {
        float start_radius = radius * (1.f - parameter);
        float end_radius = radius * (1.f - parameter - step_size);
        
        vec3 start = compute_position(origin, foot, parameter);
        vec3 end = compute_position(origin, foot, parameter + step_size);
        
        emit_tube(start, end, start_radius, end_radius);
        
        vec3 next_end = compute_position(origin, foot, parameter + 2.f * step_size);
        emit_joint(start, end, next_end, end_radius);
    }
}

void main()
{
    up_direction = normalize((NMatrix * vec4(0., 1., 0., 1.)).xyz);
    vec3 spider_position = (MVMatrix * vec4(0., 0., 0., 1.)).xyz;
    vec3 foot = gl_in[0].gl_Position.xyz;
    
    emit_leg(spider_position + 0.07 * (foot - spider_position), foot);
}
