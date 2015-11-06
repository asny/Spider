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

namespace oogl {
    
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
        GLuint get_uniform_location_with_warning(std::string variable_name);
        
        void set_uniform_variable_if_defined(std::string name, const glm::mat4& value);
        
    public:
        static glm::mat4 viewMatrix;
        static glm::mat4 projectionMatrix;
        
        GLShader(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "");
        
        void use();
        
        GLuint get_attribute_location(std::string variable_name);
        
        void set_uniform_variable(std::string name, const glm::vec3& value);
        
        void set_uniform_variable(std::string name, const glm::vec4& value);
        
        void set_uniform_variable(std::string name, const glm::mat4& value);
        
        void update_draw_matrices(const glm::mat4& modelMatrix);
    };
    
    /**
     Represents an object to draw.
     */
    class GLObject {
        
        class VertexAttribute
        {
            static const GLenum PRECISION = GL_FLOAT;
            
            int location;
            int start;
            int stride;
            int l;
            
        public:
            
            VertexAttribute(std::shared_ptr<GLShader> shader, std::string _name, int _length)
            : l(_length)
            {
                location = shader->get_attribute_location(_name);
                glEnableVertexAttribArray(location);
            }
            
            void set_data(int start_index, int stride_index, std::vector<glm::vec3> _data, std::vector<float>& data)
            {
                start = start_index * size_of_type();
                stride = stride_index * size_of_type();
                
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
            
            static int size_of_type()
            {
                if(PRECISION == GL_FLOAT)
                {
                    return sizeof(float);
                }
                if(PRECISION == GL_DOUBLE)
                {
                    return sizeof(double);
                }
                assert(false);
            }
        };
        
        std::shared_ptr<GLShader> shader;
        
        glm::mat4 modelMatrix = glm::mat4(1.);
        
        std::vector<float> data;
        
        GLuint buffer_id, array_id;
        
        std::map<std::string, VertexAttribute> attributes = std::map<std::string, VertexAttribute>();
        
        GLMaterial material;
        
        GLenum drawmode;
        
        int no_vertices = 0;
        
    public:
        
        GLObject(std::shared_ptr<GLShader> _shader, const GLMaterial& _material, GLenum _drawmode = GL_TRIANGLES);
        
        void initialize_vertex_attributes(std::vector<std::string> attribute_names);
        
        void set_vertex_attribute(std::string attribute_name, const std::vector<glm::vec3>& _data);
        
        void finalize_vertex_attributes();
        
        template<typename T>
        void set_uniform_variable(std::string name, const T& value)
        {
            shader->use();
            shader->set_uniform_variable(name, value);
        }
        
        void draw();
        
        void set_model_matrix(glm::mat4 _modelMatrix)
        {
            modelMatrix = _modelMatrix;
        }
    };
    
    /**
     A wrapper for OpenGL.
     */
    class GLWrapper {
        
    public:
        
        static void cull_backface(bool enable = true)
        {
            if(enable)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }
            else {
                glDisable(GL_CULL_FACE);
            }
        }
        
        static void initialize();
        
        /**
         Reshape the window.
         */
        static void set_screen_size(int width, int height);
        
        /**
         Set the camera/eye.
         */
        static void set_view(const glm::vec3& eyePosition, const glm::vec3& eyeDirection);
        
        /**
         Initialize drawing the objects. Should be called before any draw calls to a GLObject.
         */
        static void initialize_draw();
    };
}
