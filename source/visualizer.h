//
//  visualizer.h
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

// GLM
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
    GLShader(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "");
    
    GLuint get_shader_id()
    {
        return shader_id;
    }
    
    void set_uniform_variable(std::string name, const glm::vec3& value);
    void set_uniform_variable(std::string name, const glm::mat4& value);
};

/**
 Represents an object to draw.
 */
class GLObject {
    
    GLShader shader;
    
    std::vector<double> data;
    
    GLuint array_id, buffer_id;
    GLuint position_att, vector_att;
    
    GLMaterial material;
    
    void update_material();
    
public:
    
    GLObject(const GLShader& _shader, const GLMaterial& _material);
    
    void update_data(std::vector<glm::vec3> _data);
    
    void draw(GLenum mode = GL_TRIANGLES);
    
};

/**
 A visualizer which handles all draw functionality using OpenGL.
 */
class Visualizer {
    
    int WIDTH, HEIGHT;
    std::vector<GLShader> shaders;
    std::vector<GLObject> objects;
    
    // Uniform variables
    glm::mat4 projectionMatrix, viewMatrix, modelMatrix = glm::mat4();
    glm::vec3 center = glm::vec3(0.f);
    
public:
    
    Visualizer();
    
    void add_shader(GLShader shader)
    {
        shaders.push_back(shader);
    }
    
    void add_object(GLObject object)
    {
        objects.push_back(object);
    }
    
    /**
     Reshape the window.
     */
    void reshape(int width, int height);
    
    /**
     Set the position of the camera/eye.
     */
    void set_eye_position(const glm::vec3& eyePosition);
    
    /**
     Set the position of the light source.
     */
    void set_light_position(const glm::vec3& lightPosition);
    
    /**
     Draw the objects.
     */
    void draw();
};