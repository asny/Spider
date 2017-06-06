#version 330

uniform mat4 VPMatrix;
uniform mat4 MMatrix;
uniform float time;
uniform int noWaves;
uniform float amplitude[8];
uniform float wavelength[8];
uniform float speed[8];
uniform vec2 direction[8];
uniform float steepness[8];

in vec3 position;
in vec2 uv_coordinates;

out vec2 coords;
out vec3 nor;
out vec3 pos;

const float pi = 3.14159;

void main()
{
    pos = (MMatrix * vec4(position, 1.)).xyz;
    pos.y = 0.;
    nor = vec3(0., 1., 0.);
    
    // Offset position
    for (int i = 0; i < noWaves; ++i)
    {
        float frequency = 2*pi/wavelength[i];
        float phase = speed[i] * frequency;
        float theta = dot(direction[i], pos.xz);
        float a = theta * frequency + time * phase;
        float sin_a = sin(a);
        float cos_a = cos(a);
        pos.y += amplitude[i] * sin_a;
        pos.x += steepness[i] * amplitude[i] * direction[i].x * cos_a;
        pos.z += steepness[i] * amplitude[i] * direction[i].y * cos_a;
        
        nor.y -= steepness[i] * frequency * amplitude[i] * sin_a;
        nor.x -= direction[i].x * frequency * amplitude[i] * cos_a;
        nor.z -= direction[i].y * frequency * amplitude[i] * cos_a;
    }
    
    gl_Position = VPMatrix * vec4(pos, 1.);
    coords = uv_coordinates;
}
