#version 150

layout (points) in;
layout (triangle_strip, max_vertices = 264) out;

uniform mat4 NMatrix;
uniform mat4 MVMatrix;
uniform mat4 PMatrix;

out vec3 pos;
out vec3 nor;

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

void emit_joint(vec3 start, vec3 joint, vec3 end, float radius, float size)
{
    const float step_size = 0.2f;
    
    vec3 dir1 = joint - start;
    vec3 dir2 = end - joint;
    vec3 current_point = start;
    for (float t = 0.f; t <= 1.f; t += step_size)
    {
        vec3 dir = (1.f - t) * dir1 + t * dir2;
        vec3 next_point = current_point + 2.f * step_size * dir;
        emit_tube(current_point, next_point, radius, radius);
        current_point = next_point;
    }
}

void emit_leg(vec3 origin, vec3 foot)
{
    const float origin_radius = 0.05f;
    const float joint_size = 0.1f;
    vec3 vec = foot - origin;
    
    const float joint1_factor = 0.33f;
    const float joint1_radius = (1.f - joint1_factor) * origin_radius;
    vec3 joint1 = origin + joint1_factor * vec + func(joint1_factor) * up_direction;
    
    const float joint2_factor = 0.66f;
    const float joint2_radius = (1.f - joint2_factor) * origin_radius;
    vec3 joint2 = origin + joint2_factor * vec + func(joint2_factor) * up_direction;
    
    vec3 joint1_start = joint1 + 0.5f * joint_size * normalize(origin - joint1);
    vec3 joint1_end = joint1 + 0.5f * joint_size * normalize(joint2 - joint1);
    vec3 joint2_start = joint2 + 0.5f * joint_size * normalize(joint1 - joint2);
    vec3 joint2_end = joint2 + 0.5f * joint_size * normalize(foot - joint2);
    
    // Emit legs
    emit_tube(origin, joint1_start, origin_radius, joint1_radius);
    emit_tube(joint1_end, joint2_start, joint1_radius, joint2_radius);
    emit_tube(joint2_end, foot, joint2_radius, 0.f);
    
    // Emit joints
    emit_joint(joint1_start, joint1, joint1_end, joint1_radius, joint_size);
    emit_joint(joint2_start, joint2, joint2_end, joint2_radius, joint_size);
}

void main()
{
    up_direction = normalize((NMatrix * vec4(0., 1., 0., 1.)).xyz);
    vec3 spider_position = (MVMatrix * vec4(0., 0., 0., 1.)).xyz;
    vec3 foot = gl_in[0].gl_Position.xyz;
    
    emit_leg(spider_position + 0.07 * (foot - spider_position), foot);
}
