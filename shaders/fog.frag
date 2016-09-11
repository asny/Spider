#version 150

in vec2 posLocal;

out vec4 fragColour;

void main(void)
{
    float dist = length(posLocal);
    if(dist > 1.f)
    {
        discard;
    }
    float opacity = 0.25f * (1.f - dist);
    fragColour = vec4(1., 1., 1., opacity);
}
