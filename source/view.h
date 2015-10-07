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
#include <map>

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
        const GLenum PRECISION = GL_FLOAT;
        
        int location;
        int start;
        int stride;
        int l;
    public:
        
        VertexAttribute(GLShader shader, std::string _name, int _length)
            : l(_length)
        {
            location = shader.get_attribute_location(_name);
            glEnableVertexAttribArray(location);
        }
        
        void set_data(int start_index, int stride_index, std::vector<glm::vec3> _data, std::vector<float>& data)
        {
            start = start_index * sizeof(float);
            stride = stride_index * sizeof(float);
            
            for (int i = 0; i < _data.size(); i++)
            {
                glm::vec3 vec = _data[i];
                data[start_index + i*stride_index] = vec.x;
                data[start_index + i*stride_index + 1] = vec.y;
                data[start_index + i*stride_index + 2] = vec.z;
            }
        }
        
        void use()
        {
            glVertexAttribPointer(location, l, PRECISION, GL_FALSE, stride, (const GLvoid *)start);
        }
        
        int length()
        {
            return l;
        }
        
        int size()
        {
            if(PRECISION == GL_FLOAT)
            {
                return l * sizeof(float);
            }
            if(PRECISION == GL_DOUBLE)
            {
                return l * sizeof(double);
            }
            assert(false);
        }
    };
    
    GLShader shader;
    
    std::vector<float> data;
    
    GLuint buffer_id;
    std::map<std::string, VertexAttribute> attributes = std::map<std::string, VertexAttribute>();
    
    GLMaterial material;
    
    GLenum drawmode;
    
    int no_vertices = 0;
    
public:
    
    GLObject()
    {
        
    }
    
    GLObject(const GLShader& _shader, const GLMaterial& _material, GLenum _drawmode = GL_TRIANGLES);
    
    void initialize_vertex_attributes(std::vector<std::string> attribute_names);
    
    void set_vertex_attribute(std::string attribute_name, const std::vector<glm::vec3>& _data);
    
    void finalize_vertex_attributes();
    
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