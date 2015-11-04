#version 150

layout (lines) in;
layout (triangle_strip, max_vertices = 264) out;

uniform mat4 PMatrix;
uniform vec3 spiderPosition;
uniform vec3 wind;

out vec3 pos;
out vec3 nor;

const float half_width = 0.025f;
const vec3 up_direction = vec3(0.f, 1.f, 0.f);

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
    vec3 vec = top - origin1;
    vec3 tangent_t;
    if ( parameter < 0.0001f)
    {
        tangent_t = up_direction;
    }
    else {
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

vec3 bend_straw(float power, vec3 straw)
{
    float l = length(straw);
    vec3 bend_direction = normalize(cross(cross(up_direction, straw), straw));
    return l * normalize(straw + power * bend_direction);
}

void main()
{
    vec3 origin = gl_in[0].gl_Position.xyz;
    vec3 straw = gl_in[1].gl_Position.xyz - origin;
    float l = length(straw);
    float distance_to_spider = distance(origin.xz, spiderPosition.xz);
    
    // Bend straw for wind
    float wind_power = dot(straw, wind);
    straw = bend_straw(wind_power, straw);
    
    // Bend straw for spider
    float spider_power = 0.2f * l * max(10.f-distance_to_spider, 0.f);
    straw = bend_straw(spider_power, straw);
    
    // Compute top and corners
    vec3 leave_direction = normalize(cross(up_direction, straw));
    vec3 bend_direction = normalize(cross(leave_direction, up_direction));
    vec3 corner1 = origin + half_width * leave_direction - half_width * bend_direction;
    vec3 corner2 = origin - half_width * leave_direction - half_width * bend_direction;
    vec3 top = origin + straw;
    
    // Emit vertices
    float step_size = 1.f / max(ceil(40.f - 2.f * distance_to_spider), 4);
    emit_straw_half(corner1, origin, top, step_size);
    emit_straw_half(origin, corner2, top, step_size);
}
