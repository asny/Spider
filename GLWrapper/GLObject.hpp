//
//  GLObject.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 07/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"
#include "GLShader.hpp"
#include "GLTexture.hpp"

namespace oogl {
    struct GLMaterial
    {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
    };
    
    /**
     Represents an object to draw.
     */
    class GLObject {
        
        class VertexAttribute
        {
            static const GLenum PRECISION = GL_FLOAT;
            
            int location;
            int l;
            
            int start_index;
            int stride_index;
            
        public:
            
            VertexAttribute(std::shared_ptr<GLShader> shader, std::string _name, int _start_index, int _stride_index, int _length)
            : start_index(_start_index), stride_index(_stride_index), l(_length)
            {
                location = shader->get_attribute_location(_name);
                glEnableVertexAttribArray(location);
            }
            
            void set_data(std::vector<glm::vec3> _data, std::vector<float>& data)
            {
                data.resize(stride_index * static_cast<int>(_data.size())); // Make sure that data has the correct size.
                
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
                glVertexAttribPointer(location, l, PRECISION, GL_FALSE, stride_index * size_of_type(), (const GLvoid *)(start_index * size_of_type()));
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
        
        std::shared_ptr<GLTexture> texture;
        
        GLenum drawmode;
        
        int no_vertices = 0;
        
    public:
        
        GLObject(std::shared_ptr<GLShader> _shader, const GLMaterial& _material, GLenum _drawmode = GL_TRIANGLES, std::shared_ptr<GLTexture> _texture = nullptr);
        
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
}
