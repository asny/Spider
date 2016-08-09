//
//  GLVertexAttribute.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 09/08/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLShader.hpp"

namespace oogl
{
    class GLVertexAttribute
    {
    public:
        GLVertexAttribute(std::string _name, int _size, std::shared_ptr<GLShader> shader) : name(_name), size(_size)
        {
            // Generate and bind buffer
            glGenBuffers(1, &buffer_id);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            
            // Initialize attribute
            GLuint location = shader->get_attribute_location(name);
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, size * sizeof(float), (const GLvoid *)(0));
            check_gl_error();
        }
        
        template<class T>
        void update_data(const std::vector<T>& data)
        {
            auto floats = std::vector<float>(size * data.size());
            for(int i = 0; i < data.size(); i++)
            {
                const T& vec = data.at(i);
                for(int j = 0; j < size; j++)
                {
                    floats[i * size + j] = vec[j];
                }
            }
            // Bind buffer and send data
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            glBufferData(GL_ARRAY_BUFFER, floats.size() * sizeof(float), &floats[0], GL_STATIC_DRAW);
            check_gl_error();
        }
        
        const std::string& get_name()
        {
            return name;
        }
        
    private:
        GLuint buffer_id;
        std::string name;
        int size;
    };
}