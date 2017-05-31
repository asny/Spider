#version 330

uniform mat4 MMatrix;
uniform float time;
uniform vec3 windDirection;

in vec3 position;
in vec2 uv_coordinates;

out VertexData {
    vec2 coords;
} VertexOut;

void main()
{
    // Calculate height
    float wave = sin(mod(2. * (position.x * windDirection.x + position.z * windDirection.z + time), 6.28));
    float height = 0.1 * wave;
    
    VertexOut.coords = uv_coordinates;
    gl_Position = MMatrix * vec4(position.x, height, position.z, 1.);
}
