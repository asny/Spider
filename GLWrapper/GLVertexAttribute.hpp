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
        GLVertexAttribute(GLuint location, std::shared_ptr<geogo::Attribute<geogo::VertexID, ValueType>> _attribute)
            : attribute(_attribute)
        {
            // Generate and bind buffer
            glGenBuffers(1, &buffer_id);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            
            // Initialize attribute
            size = sizeof(ValueType) / sizeof(float);
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, size * sizeof(float), (const GLvoid *)(0));
            check_gl_error();
            
            std::function<void()> on_attribute_changed = std::bind(&GLVertexAttribute::deprecate, this);
            _attribute->subscribe_to(on_attribute_changed);
        }
        
        bool is_up_to_date()
        {
            return up_to_date;
        }
        
        void add_data_at(const geogo::VertexID& id)
        {
            ValueType vec = attribute->at(id);
            for(int j = 0; j < size; j++)
            {
                data.push_back(vec[j]);
            }
        }
        
        void send_data()
        {
            // Bind buffer and send data
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
            check_gl_error();
            
            up_to_date = true;
            data.clear();
        }
        
    private:
        
        void deprecate()
        {
            up_to_date = false;
        }
        
        bool up_to_date = false;
        std::shared_ptr<const geogo::Attribute<geogo::VertexID, ValueType>> attribute;
        GLuint buffer_id;
        int size;
        std::vector<float> data;
    };
}