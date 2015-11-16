#version 150

layout (lines) in;
layout (triangle_strip, max_vertices = 264) out;

uniform mat4 NMatrix;
uniform mat4 MVMatrix;
uniform mat4 PMatrix;

uniform vec3 spiderPosition;
uniform vec3 wind;

out vec3 pos;
out vec3 nor;

const float half_width = 0.015f;

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
    return sqrt(x);
}

float dfunc(float x)
{
    return 0.5f / sqrt(x);
}

vec3 compute_position(vec3 origin, vec3 top, float parameter)
{
    vec3 vec = top - origin;
    return origin + vec3(parameter * vec.x, func(parameter) * vec.y, parameter * vec.z);
}

vec3 compute_normal(vec3 origin1, vec3 origin2, vec3 top, float parameter)
{
    vec3 tangent_t;
    if ( parameter < 0.0001f)
    {
        tangent_t = up_direction;
    }
    else {
        vec3 vec = top - origin1;
        tangent_t = normalize(vec3(vec.x, dfunc(parameter) * vec.y, vec.z));
    }
    return normalize(cross(normalize(origin2 - origin1), tangent_t));
}

void emit_straw_half(vec3 origin1, vec3 origin2, vec3 top, float step_size)
{
    for (float parameter = 0.f; parameter < 1.f; parameter += step_size)
    {
        vec3 p1 = compute_position(origin1, top, parameter);
        vec3 p2 = compute_position(origin2, top, parameter);
        
        vec3 n = compute_normal(origin1, origin2, top, parameter);
        
        emit_vertex(p1, n);
        emit_vertex(p2, n);
    }
    EndPrimitive();
}

// Bend straw for wind and spider
vec3 compute_top(vec3 origin, vec3 straw, vec3 bend_direction, vec3 spider_position, float distance_to_spider)
{
    const float spider_power_radius = 0.8f;
    if (distance_to_spider < 0.4 * spider_power_radius)
    {
        return origin;
    }
    
    float l = length(straw);
    
    // Apply wind or spider forces
    if(distance_to_spider < spider_power_radius)
    {
        vec3 s = (origin - spider_position) / distance_to_spider;
        vec3 t = cross(s, up_direction);
        s = cross(up_direction, t);
        straw += 5.f * l * (spider_power_radius - distance_to_spider) * s;
    }
    else {
        vec3 w = (NMatrix * vec4(wind, 1.)).xyz;
        straw += dot(straw, w) * bend_direction;
    }
    straw = l * normalize(straw);
    
    return origin + straw;
}

void main()
{
    // Compute variables
    up_direction = (NMatrix * vec4(0., 1., 0., 1.)).xyz;
    
    vec3 origin = gl_in[0].gl_Position.xyz;
    vec3 straw = gl_in[1].gl_Position.xyz - origin;
    vec3 straw_direction = normalize(straw);
    vec3 leave_direction = normalize(cross(up_direction, straw_direction));
    vec3 bend_direction = normalize(cross(leave_direction, up_direction));
    
    // Find distance to spider
    vec3 spider_position = (MVMatrix * vec4(spiderPosition, 1.)).xyz;
    float distance_to_spider = distance(origin, spider_position);
    
    // Compute top
    vec3 top = compute_top(origin, straw, bend_direction, spider_position, distance_to_spider);
    
    // Compute corners
    vec3 corner1 = origin + half_width * leave_direction - 0.5 * half_width * bend_direction;
    vec3 corner2 = origin - half_width * leave_direction - 0.5 * half_width * bend_direction;
    
    // Emit vertices
    float step_size = 1.f / max(ceil(30.f - 3.f * distance_to_spider), 4);
    emit_straw_half(corner1, origin, top, step_size);
    emit_straw_half(origin, corner2, top, step_size);
}
