//
//  GLShader.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 07/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include <vector>

#include "GLShader.hpp"
#include "Reader.hpp"

using namespace oogl;
using namespace std;
using namespace glm;

// Create a GLSL program object from vertex and fragment shader files
GLuint init_shader(const char* vShaderFile, const char* fShaderFile, const char* outputAttributeName, const char* gShaderFile)
{
    struct Shader {
        string  filename;
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
        s.source = Reader::read_shader_source( s.filename );
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

GLShader::GLShader(std::vector<VertexAttribute> _attributes, string vertexShaderFilename, string fragmentShaderFilename, string geometryShaderFilename)
    : attributes(_attributes)
{
    if(geometryShaderFilename.length() != 0)
    {
        shader_id = init_shader(&vertexShaderFilename[0], &fragmentShaderFilename[0], "fragColour", &geometryShaderFilename[0]);
    }
    else {
        shader_id = init_shader(&vertexShaderFilename[0], &fragmentShaderFilename[0], "fragColour", nullptr);
    }
    
    for (auto attribute : attributes)
    {
        stride += attribute.size;
    }
    
    check_gl_error();
}

void GLShader::use()
{
    glUseProgram(shader_id);
}

GLuint GLShader::get_attribute_location(std::string variable_name)
{
    GLuint attributeLocation = glGetAttribLocation(shader_id, &variable_name[0]);
    if (attributeLocation == NULL_LOCATION) {
        std::cerr << "Shader did not contain the '" << variable_name << "' attribute variable."<<std::endl;
    }
    return attributeLocation;
}

void GLShader::initialize_vertex_attributes()
{
    for (auto attribute : attributes)
    {
        GLuint location = get_attribute_location(attribute.name);
        glEnableVertexAttribArray(location);
    }
}

void GLShader::use_vertex_attributes()
{
    int start_index = 0;
    for (auto attribute : attributes)
    {
        GLuint location = get_attribute_location(attribute.name);
        glVertexAttribPointer(location, attribute.size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const GLvoid *)(start_index * sizeof(float)));
        start_index += attribute.size;
    }
}

GLuint GLShader::get_uniform_location(std::string variable_name)
{
    GLuint uniformLocation = glGetUniformLocation(shader_id, &variable_name[0]);
    if (uniformLocation == NULL_LOCATION) {
        std::cerr << "Shader did not contain the '" << variable_name << "' uniform variable."<<std::endl;
    }
    return uniformLocation;
}

void GLShader::set_uniform_variable_if_defined(std::string name, const mat4& value)
{
    GLuint uniformLocation = glGetUniformLocation(shader_id, &name[0]);
    if (uniformLocation != NULL_LOCATION) {
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &value[0][0]);
    }
}

void GLShader::set_uniform_variable(std::string name, int value)
{
    glUniform1i(get_uniform_location(name), value);
}

void GLShader::set_uniform_variable(std::string name, const vec2& value)
{
    glUniform2fv(get_uniform_location(name), 1, &value[0]);
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
