#version 150

in vec3 center;
in vec3 pos;
out vec4 fragColour;

void main(void)
{
    float dist = distance(center, pos);
    float opacity = 0.5f * max(1.f - 2.f * dist, 0.f);
    fragColour = vec4(1., 0., 0., opacity);
}
