#version 330

uniform mat4 VPMatrix;
uniform mat4 MMatrix;
uniform float time;
uniform int noWaves;
uniform float amplitude[8];
uniform float wavelength[8];
uniform float speed[8];
uniform vec2 direction[8];
uniform sampler2D noiseTexture;

in vec3 position;
in vec2 uv_coordinates;

out vec2 coords;
out vec3 nor;
out vec3 pos;

const float pi = 3.14159;

float dWavedx(int i, float x, float y)
{
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy(int i, float x, float y)
{
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].y * frequency;
    return A * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float y)
{
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < noWaves; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, 1.0, -dy);
    return normalize(n);
}

float wave(int i, float x, float y)
{
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    return amplitude[i] * sin(theta * frequency + time * phase);
}

float waveHeight(float x, float y)
{
    float height = 0.0;
    for (int i = 0; i < noWaves; ++i)
        height += wave(i, x, y);
    return height;
}

void main()
{
    pos = (MMatrix * vec4(position, 1.)).xyz;
    float noise = sin(3.14 * texture(noiseTexture, uv_coordinates).x + time);
    pos.y = waveHeight(pos.x, pos.z) + 0.05 * noise;
    nor = waveNormal(pos.x, pos.z);
    gl_Position = VPMatrix * vec4(pos, 1.);
    coords = uv_coordinates;
}
