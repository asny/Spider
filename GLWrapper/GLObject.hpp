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
        std::shared_ptr<geogo::Geometry> geometry;
        std::shared_ptr<GLMaterial> material;
        
        GLuint array_id;
        std::vector<GLVertexAttribute> vertex_attributes;
        GLenum drawmode;
        
        // Transformation
        glm::mat4 modelMatrix = glm::mat4(1.);
        
        /**
         Updates the data of the given vertex attribute.
         */
        void update_vertex_attribute(GLVertexAttribute& attribute);
        
    public:
        
        GLObject(std::shared_ptr<geogo::Geometry> geometry, std::shared_ptr<GLMaterial> material, GLenum drawmode = GL_TRIANGLES);
        
        std::shared_ptr<geogo::Geometry> get_geometry()
        {
            return geometry;
        }
        
        /**
         Updates the data of all the vertex attributes.
         */
        void update_vertex_attributes();
        
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
