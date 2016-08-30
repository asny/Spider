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

GLuint GLShader::current_shader_id = -1;

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

GLShader::GLShader(string vertexShaderFilename, string fragmentShaderFilename, string geometryShaderFilename)
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
    if(current_shader_id != shader_id)
    {
        glUseProgram(shader_id);
        current_shader_id = shader_id;
    }
}

GLuint GLShader::get_attribute_location(std::string variable_name)
{
    use();
    GLuint attributeLocation = glGetAttribLocation(shader_id, &variable_name[0]);
    if (attributeLocation == NULL_LOCATION) {
        std::cerr << "Shader did not contain the '" << variable_name << "' attribute variable."<<std::endl;
    }
    return attributeLocation;
}

GLuint GLShader::get_uniform_location(std::string variable_name)
{
    use();
    GLuint uniformLocation = glGetUniformLocation(shader_id, &variable_name[0]);
    if (uniformLocation == NULL_LOCATION) {
        std::cerr << "Shader did not contain the '" << variable_name << "' uniform variable."<<std::endl;
    }
    return uniformLocation;
}