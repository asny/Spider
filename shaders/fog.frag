#version 330

in vec2 posLocal;

layout (location = 0) out vec4 color;

void main(void)
{
    float dist = length(posLocal);
    if(dist > 1.f)
    {
        discard;
    }
    float opacity = 0.25f * (1.f - dist);
    color = vec4(1., 1., 1., opacity);
}
