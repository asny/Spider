//
//  GLVertexAttribute.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 09/08/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"
#include "Geometry.hpp"

namespace oogl
{   
    template<class ValueType>
    class GLVertexAttribute
    {
    public:
        GLVertexAttribute(int _size, GLuint location, std::shared_ptr<geogo::Attribute<geogo::VertexID, ValueType>> _attribute)
            : size(_size), attribute(_attribute)
        {
            // Generate and bind buffer
            glGenBuffers(1, &buffer_id);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            
            // Initialize attribute
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, size * sizeof(float), (const GLvoid *)(0));
            check_gl_error();
            
            using namespace std::placeholders;
            std::function<void()> callback = std::bind(&GLVertexAttribute::deprecate, this);
            attribute->subscribe(callback);
        }
        
        void update(std::shared_ptr<geogo::Geometry> geometry)
        {
            if(is_up_to_date)
                return;
            
            auto data = std::vector<ValueType>();
            for(auto face = geometry->faces_begin(); face != geometry->faces_end(); face = face->next())
            {
                data.push_back(attribute->get(*face->v1()));
                data.push_back(attribute->get(*face->v2()));
                data.push_back(attribute->get(*face->v3()));
            }
            update_data(data);
            is_up_to_date = true;
        }
        
    private:
        
        void update_data(const std::vector<ValueType>& data)
        {
            auto floats = std::vector<float>(size * data.size());
            for(int i = 0; i < data.size(); i++)
            {
                const ValueType& vec = data.at(i);
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
        
        void deprecate()
        {
            is_up_to_date = false;
        }
        
        bool is_up_to_date = false;
        std::shared_ptr<geogo::Attribute<geogo::VertexID, ValueType>> attribute;
        GLuint buffer_id;
        
        int size;
    };
}