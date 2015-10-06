//
//  view.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "view.h"
#include "gtc/matrix_inverse.hpp"

using namespace glm;

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

const static unsigned int NULL_LOCATION = -1;

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

void GLShader::use()
{
    glUseProgram(shader_id);
}

GLuint GLShader::get_uniform_location(std::string variable_name)
{
    GLuint uniformLocation = glGetUniformLocation(shader_id, &variable_name[0]);
    if (uniformLocation == NULL_LOCATION) {
        std::cerr << "Shader did not contain the '" << variable_name << "' uniform variable."<<std::endl;
    }
    return uniformLocation;
}

GLuint GLShader::get_attribute_location(std::string variable_name)
{
    GLuint attributeLocation = glGetAttribLocation(shader_id, &variable_name[0]);
    if (attributeLocation == NULL_LOCATION) {
        std::cerr << "Shader did not contain the '" << variable_name << "' attribute variable."<<std::endl;
    }
    return attributeLocation;
}

void GLShader::set_uniform_variable(std::string name, const vec3& value)
{
    glUniform3fv(get_uniform_location(name), 1, &value[0]);
}

void GLShader::set_uniform_variable(std::string name, const vec4& value)
{
    glUniform4fv(get_uniform_location(name), 1, &value[0]);
}

void GLShader::set_uniform_variable(std::string name, const mat4& value)
{
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &value[0][0]);
}


GLObject::GLObject(const GLShader& _shader, const GLMaterial& _material, GLenum _drawmode)
: shader(_shader), material(_material), drawmode(_drawmode)
{
    // Generate arrays and buffers
    GLuint array_id;
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    
    // Initialize shader attributes
    position_att = shader.get_attribute_location("position");
    vector_att = shader.get_attribute_location("vector");
    
    glEnableVertexAttribArray(position_att);
    glEnableVertexAttribArray(vector_att);
    
    check_gl_error();
}

void GLObject::set_data(const std::vector<vec3>& _data)
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

void GLObject::draw()
{
    if(data.size() != 0)
    {
        shader.use();
        shader.set_uniform_variable("ambientMat", material.ambient);
        shader.set_uniform_variable("diffuseMat", material.diffuse);
        shader.set_uniform_variable("specMat", material.specular);
        
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        
        const int vector_size = 3*sizeof(double);
        glVertexAttribPointer(position_att, 3, GL_DOUBLE, GL_FALSE, 2*vector_size, (const GLvoid *)0);
        glVertexAttribPointer(vector_att, 3, GL_DOUBLE, GL_FALSE, 2*vector_size, (const GLvoid *)vector_size);
        
        glDrawArrays(drawmode, 0, static_cast<int>(data.size())/(2*3));
        
        check_gl_error();
    }
}

GLWrapper::GLWrapper()
{
    // Enable states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    set_view(vec3(0.f, 0.f, 5.f), vec3(0.f, 0.f, -1.f));
    
    check_gl_error();
}

void GLWrapper::reshape(int width, int height)
{
    mat4 projectionMatrix = glm::perspective(45.f, width/float(height), 0.01f, 100.f);
    glViewport(0, 0, width, height);
    
    // Send model-view matrix uniform to the shaders
    for (GLShader shader : shaders)
    {
        shader.use();
        shader.set_uniform_variable("PMatrix", projectionMatrix);
    }
    
    check_gl_error();
}

void GLWrapper::set_view(const glm::vec3& eyePosition, const glm::vec3& eyeDirection)
{
    viewMatrix = glm::lookAt(eyePosition, eyePosition + eyeDirection, glm::vec3(0., 1., 0.));
    glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
    glm::mat4 normalMatrix = glm::inverseTranspose(modelViewMatrix);
    
    // Send model-view, normal and model-view-projection matrix uniforms to the shaders
    for (GLShader shader : shaders)
    {
        shader.use();
        shader.set_uniform_variable("MVMatrix", modelViewMatrix);
        shader.set_uniform_variable("NormalMatrix", normalMatrix);
    }
    
    check_gl_error();
}

void GLWrapper::set_light_position(const vec3& lightPosition)
{
    for (GLShader shader : shaders)
    {
        shader.use();
        shader.set_uniform_variable("lightPos", lightPosition);
    }
    
    check_gl_error();
}

void GLWrapper::draw()
{
    glClearColor(1., 1., 1., 0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    check_gl_error();
}
