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

/**
 Represents a shader used to draw a GLObject.
 */
class GLShader {
    GLuint shader_id;
    
public:
    GLShader(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "");
    
    GLuint get_shader_id()
    {
        return shader_id;
    }
};

/**
 Represents an object to draw.
 */
class GLObject {
    
    GLShader shader;
    
    std::vector<double> data;
    
    GLuint array_id, buffer_id;
    GLuint position_att, vector_att;
    
    glm::vec4 ambient_mat, diffuse_mat, specular_mat;
    
public:
    
    GLObject(GLShader _shader, const glm::vec4& ambient_mat, const glm::vec4& diffuse_mat, const glm::vec4& specular_mat);
    
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