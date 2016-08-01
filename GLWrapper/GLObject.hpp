//
//  GLObject.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 07/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Geometry.hpp"
#include "GLMaterial.hpp"

namespace oogl
{
    /**
     Represents an object to draw.
     */
    class GLObject
    {
        class VertexAttribute
        {
        public:
            
            std::string name;
            int size;
        };
        
        std::shared_ptr<Geometry> geometry;
        std::shared_ptr<GLMaterial> material;
        
        // Vertex attribute data
        std::vector<float> data;
        int stride = 0;
        std::vector<VertexAttribute> attributes;
        
        // Needed for drawing
        GLuint no_vertices = 0;
        GLuint buffer_id, array_id;
        GLenum drawmode;
        
        // Transformation
        glm::mat4 modelMatrix = glm::mat4(1.);
        
        int get_attribute_start_index(std::string name)
        {
            int start_index = 0;
            for (auto attribute : attributes)
            {
                if(attribute.name == name)
                {
                    return start_index;
                }
                start_index += attribute.size;
            }
            return -1;
        }
        
        void update_vertex_attributes();
        
        template<class IDType, class ValueType>
        std::vector<std::shared_ptr<Attribute<IDType, ValueType>>> get_used_attributes(const std::vector<std::shared_ptr<Attribute<IDType, ValueType>>>& attributes)
        {
            auto attributeIds = material->get_attribute_ids();
            auto used_attributes = std::vector<std::shared_ptr<Attribute<IDType, ValueType>>>();
            for(auto attribute : attributes)
            {
                if(std::find(attributeIds.begin(), attributeIds.end(), attribute->get_id()) != attributeIds.end())
                {
                    used_attributes.push_back(attribute);
                }
            }
            return used_attributes;
        }
        
    public:
        
        GLObject(std::shared_ptr<GLMaterial> material, std::shared_ptr<Geometry> geometry);
        
        GLObject(std::vector<VertexAttribute> attributes, std::shared_ptr<GLMaterial> material, GLenum drawmode = GL_TRIANGLES);
        
        /**
         Updates the value of the vertex attribute with the given name.
         */
        void update_vertex_attribute(std::string name, const std::vector<glm::vec2>& _data);
        
        /**
         Updates the value of the vertex attribute with the given name.
         */
        void update_vertex_attribute(std::string name, const std::vector<glm::vec3>& _data);
        
        /**
         Should be called after all vertex attributes has been updated.
         */
        void finalize_vertex_attributes();
        
        /**
         Updates the value of the uniform variable with the given name.
         */
        template<typename T>
        void update_uniform_variable(std::string name, const T& value)
        {
            material->set_uniform_variable(name, value);
        }
        
        /**
         Draws the object.
         */
        void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        
        /**
         Set the model matrix.
         */
        void set_model_matrix(glm::mat4 _modelMatrix)
        {
            modelMatrix = _modelMatrix;
        }
    };
}
