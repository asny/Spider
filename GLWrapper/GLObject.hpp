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
#include "GLVertexAttribute.hpp"
#include "GLUniform.hpp"

namespace oogl
{
    /**
     Represents an object to draw.
     */
    class GLObject
    {
        std::shared_ptr<geogo::Geometry> geometry;
        std::shared_ptr<GLMaterial> material;
        
        GLuint array_id;
        std::vector<std::shared_ptr<GLVertexAttribute<glm::vec2>>> vec2_vertex_attributes;
        std::vector<std::shared_ptr<GLVertexAttribute<glm::vec3>>> vec3_vertex_attributes;
        std::vector<GLUniform<glm::vec3>> vec3_uniforms;
        std::vector<GLUniform<glm::mat4>> mat4_uniforms;
        
        // Transformation
        glm::mat4 modelMatrix = glm::mat4(1.);
        
        template<class T>
        void update_attribute(std::shared_ptr<GLVertexAttribute<T>> attribute)
        {
            if(attribute->is_up_to_date())
                return;
            if(geometry->get_no_faces() > 0)
            {
                for(auto face = geometry->faces_begin(); face != geometry->faces_end(); face = face->next())
                {
                    attribute->add_data_at(*face->v1());
                    attribute->add_data_at(*face->v2());
                    attribute->add_data_at(*face->v3());
                }
            }
            else if(geometry->get_no_edges() > 0)
            {
                for(auto edge = geometry->edges_begin(); edge != geometry->edges_end(); edge = edge->next())
                {
                    attribute->add_data_at(*edge->v1());
                    attribute->add_data_at(*edge->v2());
                }
            }
            else if(geometry->get_no_vertices() > 0)
            {
                for(auto vertex = geometry->vertices_begin(); vertex != geometry->vertices_end(); vertex = vertex->next())
                {
                    attribute->add_data_at(*vertex);
                }
            }
            attribute->send_data();
        }
        
    public:
        
        GLObject(std::shared_ptr<geogo::Geometry> geometry, std::shared_ptr<GLMaterial> material);
        
        void use_attribute(std::string name, std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec2>> attribute)
        {
            glBindVertexArray(array_id);
            auto location = material->get_attribute_location(name);
            vec2_vertex_attributes.push_back(std::shared_ptr<GLVertexAttribute<glm::vec2>>(new GLVertexAttribute<glm::vec2>(location, attribute)));
        }
        
        void use_attribute(std::string name, std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> attribute)
        {
            glBindVertexArray(array_id);
            auto location = material->get_attribute_location(name);
            vec3_vertex_attributes.push_back(std::shared_ptr<GLVertexAttribute<glm::vec3>>(new GLVertexAttribute<glm::vec3>(location, attribute)));
        }
        
        void use_uniform(const std::string& name, const std::shared_ptr<glm::vec3> value)
        {
            auto location = material->get_uniform_location(name);
            if(location != NULL_LOCATION)
                vec3_uniforms.push_back(GLUniform<glm::vec3>(location, value));
        }
        
        void use_uniform(const std::string& name, const std::shared_ptr<glm::mat4> value)
        {
            auto location = material->get_uniform_location(name);
            if(location != NULL_LOCATION)
                mat4_uniforms.push_back(GLUniform<glm::mat4>(location, value));
        }
        
        /**
         Draws the object.
         */
        void draw();
        
        /**
         Set the model matrix.
         */
        void set_model_matrix(glm::mat4 _modelMatrix)
        {
            modelMatrix = _modelMatrix;
        }
        
        const glm::mat4& get_model()
        {
            return modelMatrix;
        }
    };
}
