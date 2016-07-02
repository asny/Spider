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
out float ambientFactor;

const float half_width = 0.015f;

vec3 up_direction;

float func(float x)
{
    return -x*x + x;
}

float dfunc(float x)
{
    return -2.f*x + 1.f;
}

vec3 compute_position(vec3 origin, vec3 top, float parameter)
{
    return origin + parameter * (top - origin) + func(parameter) * up_direction;
}

vec3 compute_normal(vec3 origin, vec3 corner, vec3 top, float parameter)
{
    vec3 tangent = top - origin + dfunc(parameter) * up_direction;
    return normalize(cross(corner - origin, tangent));
}

void emit_straw_half(vec3 origin, vec3 corner, vec3 top, float step_size)
{
    for (float parameter = 0.f; parameter < 1.f; parameter += step_size)
    {
        nor = compute_normal(origin, corner, top, parameter);
        
        pos = compute_position(origin, top, parameter);
        ambientFactor = 0.6;
        gl_Position = PMatrix * vec4(pos, 1.);
        EmitVertex();
        
        pos = compute_position(corner, top, parameter);
        ambientFactor = 1.;
        gl_Position = PMatrix * vec4(pos, 1.);
        EmitVertex();
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
    vec3 origin = gl_in[0].gl_Position.xyz;
    // Check if the straw is behind the camera
    if(origin.z > 0.)
        return;
    vec3 straw = gl_in[1].gl_Position.xyz - origin;
    
    // Compute directions
    up_direction = (NMatrix * vec4(0., 1., 0., 1.)).xyz;
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
    emit_straw_half(origin, corner1, top, step_size);
    emit_straw_half(origin, corner2, top, step_size);
}
