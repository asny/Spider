//
//  visualizer.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "visualizer.h"
#include "gtc/matrix_inverse.hpp"

inline void _check_gl_error(const char *file, int line)
{
    GLenum err (glGetError());
    
    while(err!=GL_NO_ERROR) {
        std::string error;
        
        switch(err) {
            case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        
        std::cerr << "GL_" << error.c_str() <<" - "<<file<<":"<<line<<std::endl;
        err=glGetError();
    }
}

#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

using namespace glm;

const static unsigned int NULL_LOCATION = -1;

GLObject::GLObject(const GLShader& _shader, const GLMaterial& _material)
: shader(_shader), material(_material)
{
    // Generate arrays and buffers
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    
    // Initialize shader attributes
    position_att = glGetAttribLocation(shader.get_shader_id(), "position");
    if (position_att == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'position' attribute." << std::endl;
    }
    vector_att = glGetAttribLocation(shader.get_shader_id(), "vector");
    if (vector_att == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'vector' attribute." << std::endl;
    }
    
    glEnableVertexAttribArray(position_att);
    glEnableVertexAttribArray(vector_att);
    check_gl_error();
}

void GLObject::update_data(std::vector<vec3> _data)
{
    data.clear();
    for (vec3 vec : _data)
    {
        data.push_back(vec.x);
        data.push_back(vec.y);
        data.push_back(vec.z);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double)*data.size(), &data[0], GL_STATIC_DRAW);
    check_gl_error();
}

void GLObject::draw(GLenum mode)
{
    if(data.size() != 0)
    {
        glUseProgram(shader.get_shader_id());
        GLuint uniform = (GLuint) glGetUniformLocation(shader.get_shader_id(), "ambientMat");
        if (uniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'ambientMat' uniform."<<std::endl;
        }
        glUniform4fv(uniform, 1, &material.ambient[0]);
        
        uniform = (GLuint) glGetUniformLocation(shader.get_shader_id(), "diffuseMat");
        if (uniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'diffuseMat' uniform."<<std::endl;
        }
        glUniform4fv(uniform, 1, &material.diffuse[0]);
        
        uniform = (GLuint) glGetUniformLocation(shader.get_shader_id(), "specMat");
        if (uniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'specMat' uniform."<<std::endl;
        }
        glUniform4fv(uniform, 1, &material.specular[0]);
        
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        
        const int vector_size = 3*sizeof(double);
        glVertexAttribPointer(position_att, 3, GL_DOUBLE, GL_FALSE, 2*vector_size, (const GLvoid *)0);
        glVertexAttribPointer(vector_att, 3, GL_DOUBLE, GL_FALSE, 2*vector_size, (const GLvoid *)vector_size);
        
        glDrawArrays(mode, 0, static_cast<int>(data.size())/(2*3));
        check_gl_error();
    }
}

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile)
{
    FILE *filePointer;
    char *content = NULL;
    
    int count=0;
    
    if (shaderFile != NULL) {
        filePointer = fopen(shaderFile,"rt");
        
        if (filePointer != NULL) {
            
            fseek(filePointer, 0, SEEK_END);
            count = static_cast<int>(ftell(filePointer));
            rewind(filePointer);
            
            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = static_cast<int>(fread(content,sizeof(char),count,filePointer));
                content[count] = '\0';
            }
            fclose(filePointer);
        }
    }
    return content;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint init_shader(const char* vShaderFile, const char* fShaderFile, const char* outputAttributeName, const char* gShaderFile)
{
    struct Shader {
        const char*  filename;
        GLenum       type;
        GLchar*      source;
    };
    std::vector<Shader> shaders = { { vShaderFile, GL_VERTEX_SHADER, NULL }, { fShaderFile, GL_FRAGMENT_SHADER, NULL } };
    if(gShaderFile)
    {
        shaders.push_back({ gShaderFile, GL_GEOMETRY_SHADER, NULL });
    }
    
    GLuint program = glCreateProgram();
    
    for ( int i = 0; i < shaders.size(); ++i ) {
        Shader& s = shaders[i];
        s.source = readShaderSource( s.filename );
        if ( shaders[i].source == NULL ) {
            std::cerr << "Failed to read " << s.filename << std::endl;
            exit( EXIT_FAILURE );
        }
        GLuint shader = glCreateShader( s.type );
        glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
        glCompileShader( shader );
        
        GLint  compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            std::cerr << s.filename << " failed to compile:" << std::endl;
            GLint  logSize;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
            char* logMsg = new char[logSize];
            glGetShaderInfoLog( shader, logSize, NULL, logMsg );
            std::cerr << logMsg << std::endl;
            delete [] logMsg;
            
            exit( EXIT_FAILURE );
        }
        
        delete [] s.source;
        
        glAttachShader( program, shader );
    }
    
    /* Link output */
    glBindFragDataLocation(program, 0, outputAttributeName);
    
    /* link  and error check */
    glLinkProgram(program);
    
    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        std::cerr << "Shader program failed to link" << std::endl;
        GLint  logSize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
        char* logMsg = new char[logSize];
        glGetProgramInfoLog( program, logSize, NULL, logMsg );
        std::cerr << logMsg << std::endl;
        delete [] logMsg;
        
        exit( EXIT_FAILURE );
    }
    
    /* use program object */
    glUseProgram(program);
    
    return program;
}

GLShader::GLShader(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename)
{
    if(geometryShaderFilename.length() != 0)
    {
        shader_id = init_shader(&vertexShaderFilename[0], &fragmentShaderFilename[0], "fragColour", &geometryShaderFilename[0]);
    }
    else {
        shader_id = init_shader(&vertexShaderFilename[0], &fragmentShaderFilename[0], "fragColour", nullptr);
    }
    
    check_gl_error();
}

Visualizer::Visualizer()
{
    // Enable states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    check_gl_error();
}

void Visualizer::reshape(int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    projectionMatrix = glm::perspective(53.f, width/float(height), 0.01f, 1000.f);
    glViewport(0, 0, width, height);
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
    
    // Send model-view matrix uniform to the shaders
    for (GLShader shader : shaders)
    {
        glUseProgram(shader.get_shader_id());
        GLuint MVPMatrixUniform = glGetUniformLocation(shader.get_shader_id(), "MVPMatrix");
        if (MVPMatrixUniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'MVPMatrix' uniform."<<std::endl;
        }
        glUniformMatrix4fv(MVPMatrixUniform, 1, GL_TRUE, &modelViewProjectionMatrix[0][0]);
    }
    
    check_gl_error();
}

void Visualizer::set_eye_position(const glm::vec3& eyePosition)
{
    viewMatrix = glm::lookAt(eyePosition, center, glm::vec3(0., 1., 0.));
    glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
    glm::mat4 normalMatrix = glm::inverseTranspose(modelViewMatrix);
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
    
    // Send model-view, normal and model-view-projection matrix uniforms to the shaders
    for (GLShader shader : shaders)
    {
        glUseProgram(shader.get_shader_id());
        GLuint MVMatrixUniform = glGetUniformLocation(shader.get_shader_id(), "MVMatrix");
        if (MVMatrixUniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'MVMatrix' uniform."<<std::endl;
        }
        glUniformMatrix4fv(MVMatrixUniform, 1, GL_FALSE, &modelViewMatrix[0][0]);
        
        GLuint NormalMatrixUniform = glGetUniformLocation(shader.get_shader_id(), "NormalMatrix");
        if (NormalMatrixUniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'NormalMatrix' uniform."<<std::endl;
        }
        glUniformMatrix4fv(NormalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
        
        GLuint MVPMatrixUniform = glGetUniformLocation(shader.get_shader_id(), "MVPMatrix");
        if (MVPMatrixUniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'MVPMatrix' uniform."<<std::endl;
        }
        glUniformMatrix4fv(MVPMatrixUniform, 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);
    }
    
    check_gl_error();
}

void Visualizer::set_light_position(const vec3& lightPosition)
{
    for (GLShader shader : shaders)
    {
        glUseProgram(shader.get_shader_id());
        GLuint lightPosUniform = glGetUniformLocation(shader.get_shader_id(), "lightPos");
        if (lightPosUniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'lightPos' uniform."<<std::endl;
        }
        glUniform3fv(lightPosUniform, 1, &lightPosition[0]);
    }
    
    check_gl_error();
}

void Visualizer::draw()
{
    glClearColor(1., 1., 1., 0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (GLObject object : objects)
    {
        object.draw();
    }
    
    check_gl_error();
}
