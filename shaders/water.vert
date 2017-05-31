#version 330

uniform mat4 MMatrix;
uniform sampler2D noiseTexture;
uniform float time;
uniform vec3 windDirection;

in vec3 position;
in vec2 uv_coordinates;

void main()
{
    // Calculate height
    float noise = texture(noiseTexture, uv_coordinates).x - 0.5;
    float wave = sin(mod(2. * (position.x * windDirection.x + position.z * windDirection.z + time), 6.28));
    float height = 0.1 * wave + 0.2 * noise;
    
    gl_Position = MMatrix * vec4(position.x, height, position.z, 1.);
}
