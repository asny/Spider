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

const float half_width = 0.025f;

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

void main()
{
    up_direction = (NMatrix * vec4(0., 1., 0., 1.)).xyz;
    
    vec3 origin = gl_in[0].gl_Position.xyz;
    vec3 straw = gl_in[1].gl_Position.xyz - origin;
    float l = length(straw);
    vec3 straw_direction = straw/l;
    vec3 leave_direction = normalize(cross(up_direction, straw_direction));
    vec3 bend_direction = normalize(cross(leave_direction, straw_direction));
    
    // Find distance to spider
    vec3 spiderP = (MVMatrix * vec4(spiderPosition, 1.)).xyz;
    float distance_to_spider = distance(origin.xz, spiderP.xz);
    
    // Bend straw for wind and spider
    vec3 w = (NMatrix * vec4(wind, 1.)).xyz;
    float wind_power = dot(straw, w);
    float spider_power = 20.f * l * max(0.5f - distance_to_spider, 0.f);
    vec3 away_direction = vec3(origin.x - spiderP.x, 0., origin.z - spiderP.z);
    if (length(away_direction) < 0.0001)
    {
        away_direction = vec3(1.,0.,0.);
    }
    vec3 top = origin + l * normalize(straw + wind_power * bend_direction + spider_power * normalize(away_direction));
    top.y = max(origin.y, top.y);
    
    // Compute corners
    vec3 bend_direction_xz = normalize(vec3(bend_direction.x, 0.f, bend_direction.z));
    vec3 corner1 = origin + half_width * leave_direction - half_width * bend_direction_xz;
    vec3 corner2 = origin - half_width * leave_direction - half_width * bend_direction_xz;
    
    // Emit vertices
    float step_size = 1.f / max(ceil(30.f - 3.f * distance_to_spider), 4);
    emit_straw_half(corner1, origin, top, step_size);
    emit_straw_half(origin, corner2, top, step_size);
}
