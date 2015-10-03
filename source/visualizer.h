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

/**
 A visualizer which handles all draw functionality using OpenGL.
 */
class Visualizer {
    
    const static unsigned int NULL_LOCATION = -1;
    
    class GLObject {
        
        GLuint shader;
        
        std::vector<double> data;
        
        GLuint array_id, buffer_id;
        GLuint position_att, vector_att;
        
        glm::vec4 ambient_mat, diffuse_mat, specular_mat;
        
    public:
        
        GLObject(GLuint _shader, const glm::vec4& ambient_mat = glm::vec4(1.f), const glm::vec4& diffuse_mat = glm::vec4(0.f), const glm::vec4& specular_mat = glm::vec4(0.f));
        
        void update_data(std::vector<glm::vec3> _data);
        
        void draw(GLenum mode = GL_TRIANGLES);
        
    };
    
    int WIDTH, HEIGHT;
    GLuint gouraud_shader;
    
    std::unique_ptr<GLObject> interface;
    
    // Uniform variables
    glm::mat4 projectionMatrix, viewMatrix, modelMatrix = glm::mat4();
    glm::vec3 center = glm::vec3(0.f);
    
public:
    
    Visualizer(const glm::vec3& light_pos);
    
private:
    // Create a GLSL program object from vertex and fragment shader files
    GLuint init_shader(const char* vShaderFile, const char* fShaderFile, const char* outputAttributeName, const char* gShaderFile = nullptr);
    
public:
    /**
     Reshape the window.
     */
    void reshape(int width, int height);
    
    /**
     Set the position of the camera/eye.
     */
    void set_view_position(const glm::vec3& pos);
    
    /**
     Draws the objects.
     */
    void draw();
    
    /**
     Updates the data to visualize.
     */
    void update();
};