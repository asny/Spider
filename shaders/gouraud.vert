#version 150

uniform mat4 MVMatrix;
uniform mat4 PMatrix;
uniform mat4 NormalMatrix;

uniform vec3 lightPos;

uniform vec4 ambientMat;
uniform vec4 diffuseMat;
uniform vec4 specMat;

in vec3 position;
in vec3 normal;

out vec4 colourV;

void main()
{
    // Define material specs
    float specPow = 5.;
    
    // Compute vectors
    vec3 p = mat3(MVMatrix) * position;
    vec3 N = normalize(mat3(NormalMatrix) * normal);
    vec3 L = normalize(lightPos - p);
    vec3 E = normalize(-p);
    vec3 R = normalize(reflect(-L,N));
    
    // Calculate colour
    vec4 ambient = ambientMat;
    vec4 diffuse = clamp( diffuseMat * max(dot(N,L), 0.0) , 0.0, 1.0 ) ;
    vec4 spec = clamp ( specMat * pow(max(dot(R,E), 0.0), specPow) , 0.0, 1.0 );
    colourV = ambient + diffuse + spec;
    
    // Calculate position
    gl_Position = PMatrix * MVMatrix * vec4(position.xyz, 1.);
}
