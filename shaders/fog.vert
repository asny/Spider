#version 150

uniform mat4 MVMatrix;
uniform float time;

in vec3 position;
in vec3 normal;

const vec3 up = vec3(0.f, 1.f, 0.f);

void main()
{
    // Calculate position
    float cos_t = cos(time);
    float sin_t = sin(time);
    vec3 a = position;
    vec3 b = cross(a, normalize(normal));
    vec3 pos = cos_t * a + sin_t * b;
    
    gl_Position = MVMatrix * vec4(pos, 1.);
}
