#version 150

uniform float scale;

in vec3 center;
in vec3 pos;

out vec4 fragColour;

void main(void)
{
    float dist = distance(center, pos);
    if(dist > scale * 0.5f)
    {
        discard;
    }
    float opacity = 0.25f * (1.f - 2.f * dist / scale);
    fragColour = vec4(1., 1., 1., opacity);
}
