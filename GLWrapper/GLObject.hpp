//
//  GLObject.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 07/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Mesh.h"
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
        std::shared_ptr<geogo::Mesh> geometry;
        std::shared_ptr<GLMaterial> material;
        
        GLuint array_id;
        std::vector<std::shared_ptr<GLVertexAttribute<glm::vec2>>> vec2_vertex_attributes;
        std::vector<std::shared_ptr<GLVertexAttribute<glm::vec3>>> vec3_vertex_attributes;
        
        // Transformation
        std::shared_ptr<glm::mat4> modelMatrix = std::make_shared<glm::mat4>(1.);
        
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
        
        GLObject(std::shared_ptr<geogo::Mesh> geometry, std::shared_ptr<GLMaterial> material);
        
        /**
         Draws the object.
         */
        void draw();
        
        /**
         Set the model matrix.
         */
        void set_model_matrix(const std::shared_ptr<glm::mat4> _modelMatrix)
        {
            modelMatrix = _modelMatrix;
        }
        
        /**
         Set the model matrix.
         */
        void set_model_matrix(const glm::mat4& _modelMatrix)
        {
            modelMatrix = std::make_shared<glm::mat4>(_modelMatrix);
        }
        
        const glm::mat4& get_model()
        {
            return *modelMatrix;
        }
        
        const std::shared_ptr<GLMaterial> get_material()
        {
            return material;
        }
    };
}
