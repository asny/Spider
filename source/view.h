//
//  view.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <OpenGL/gl3.h>

#include <memory>
#include <iostream>
#include <vector>

#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"

struct GLMaterial
{
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

/**
 Represents a shader used to draw a GLObject.
 */
class GLShader {
    GLuint shader_id;
    
    GLuint get_uniform_location(std::string variable_name);
public:
    
    GLShader()
    {
        
    }
    
    GLShader(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "");
    
    void use();
    
    GLuint get_attribute_location(std::string variable_name);
    
    void set_uniform_variable(std::string name, const glm::vec3& value);
    
    void set_uniform_variable(std::string name, const glm::vec4& value);
    
    void set_uniform_variable(std::string name, const glm::mat4& value);
};

/**
 Represents an object to draw.
 */
class GLObject {
    
    class VertexAttribute
    {
        std::string name;
        GLuint index;
        GLint start_index;
        GLint size;
        GLenum type;
        
    public:
        VertexAttribute(GLShader shader, std::string _name, int _start_index, int _size, GLenum _type = GL_DOUBLE) : name(_name), start_index(_start_index), size(_size), type(_type)
        {
            index = shader.get_attribute_location(name);
            glEnableVertexAttribArray(index);
        }
        
        GLint get_size()
        {
            return size;
        }
        
        GLint get_index()
        {
            return index;
        }
        
        GLint get_start_index()
        {
            return start_index;
        }
        
        GLenum get_type()
        {
            return type;
        }
    };
    
    GLShader shader;
    
    std::vector<double> data;
    
    GLuint buffer_id;
    std::vector<VertexAttribute> attributes = std::vector<VertexAttribute>();
    
    GLMaterial material;
    
    GLenum drawmode;
    
    int no_vertices = 0;
    int stride = 0;
    
public:
    
    GLObject()
    {
        
    }
    
    GLObject(const GLShader& _shader, const GLMaterial& _material, GLenum _drawmode = GL_TRIANGLES);
    
    void set_data(const std::vector<glm::vec3>& _data);
    
    void draw();
};

/**
 A wrapper for OpenGL.
 */
class GLWrapper {
    
    std::vector<GLShader> shaders;
    
    // Uniform variables
    glm::mat4 modelMatrix = glm::mat4();
    
public:
    
    GLWrapper();
    
    void add_shader(GLShader shader)
    {
        shaders.push_back(shader);
    }
    
    /**
     Reshape the window.
     */
    void reshape(int width, int height);
    
    /**
     Set the camera/eye.
     */
    void set_view(const glm::vec3& eyePosition, const glm::vec3& eyeDirection);
    
    /**
     Set the position of the light source.
     */
    void set_light_position(const glm::vec3& lightPosition);
    
    /**
     Draw the objects.
     */
    void draw();
};