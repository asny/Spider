#version 150

uniform float radius;

in vec3 center;
in vec3 pos;

out vec4 fragColour;

void main(void)
{
    float dist = distance(center, pos);
    if(dist > radius)
    {
        discard;
    }
    float opacity = 0.25f * (1.f - dist / radius);
    fragColour = vec4(1., 1., 1., opacity);
}
