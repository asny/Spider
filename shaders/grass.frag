#version 330

uniform mat4 VMatrix;

uniform vec3 lightPos;

uniform vec3 ambientMat;
uniform vec3 diffuseMat;
uniform float opacity;

in vec3 pos;
in vec3 nor;
in float ambientFactor;

layout (location = 0) out vec4 color;

void main(void)
{
    // Compute vectors
    vec3 N = normalize(nor);
    if(!gl_FrontFacing)
    {
        N = -N;
    }
    vec3 L = normalize((VMatrix * vec4(lightPos, 1.f)).xyz - pos);
    
    // Calculate colour
    vec3 ambient = ambientFactor * ambientMat;
    vec3 diffuse = clamp( diffuseMat * max(dot(N,L), 0.0) , 0.0, 1.0 ) ;
    color = vec4(ambient + diffuse, opacity);
}
