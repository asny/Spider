#version 330

uniform sampler2D noiseTexture;

in vec2 dens;
in vec3 nor;
in vec3 pos;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

void main()
{
    float noise = texture(noiseTexture, pos.xz).x;
    float sand = noise + dens.x;
    if(sand > 0. && sand <= 1.)
    {
        position = vec4(pos, 1.0);
        normal = vec4(nor, 1.0);
        color = vec4(0.5, 0.3, 0.3, sand);
    }
    else {
        discard;
    }
}
