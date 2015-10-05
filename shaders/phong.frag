#version 150

uniform vec3 lightPos;

uniform vec4 ambientMat;
uniform vec4 diffuseMat;
uniform vec4 specMat;

in vec3 pos;
in vec3 nor;

out vec4 fragColour;

void main(void)
{
    // Define material specs
    float specPow = 5.;
    
    // Compute vectors
    vec3 N = normalize(nor);
    vec3 L = normalize(lightPos - pos);
    vec3 E = normalize(-pos);
    vec3 R = normalize(reflect(-L,N));
    
    // Calculate colour
    vec4 ambient = ambientMat;
    vec4 diffuse = clamp( diffuseMat * max(dot(N,L), 0.0)  , 0.0, 1.0 ) ;
    vec4 spec = clamp ( specMat * pow(max(dot(R,E),0.0), 0.3*specPow) , 0.0, 1.0 );
    fragColour = ambient + diffuse + spec;
}
