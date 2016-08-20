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
        std::vector<GLVertexAttribute<glm::vec2>> vec2_vertex_attributes;
        std::vector<GLVertexAttribute<glm::vec3>> vec3_vertex_attributes;
        GLenum drawmode;
        
        // Transformation
        glm::mat4 modelMatrix = glm::mat4(1.);
        
    public:
        
        GLObject(std::shared_ptr<geogo::Geometry> geometry, std::shared_ptr<GLMaterial> material, std::map<std::string, std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec2>>> custom_vec2_attributes, std::map<std::string, std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>>> custom_vec3_attributes, GLenum drawmode = GL_TRIANGLES);
        
        GLObject(std::shared_ptr<geogo::Geometry> geometry, std::shared_ptr<GLMaterial> material, GLenum drawmode = GL_TRIANGLES) : GLObject(geometry, material, {}, {}, drawmode)
        {
            
        }
        
        GLObject(std::shared_ptr<geogo::Geometry> geometry, std::shared_ptr<GLMaterial> material, std::map<std::string, std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>>> custom_vec3_attributes, GLenum drawmode = GL_TRIANGLES) : GLObject(geometry, material, {}, custom_vec3_attributes, drawmode)
        {
            
        }
        
        
        GLObject(std::shared_ptr<geogo::Geometry> geometry, std::shared_ptr<GLMaterial> material, std::map<std::string, std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec2>>> custom_vec2_attributes, GLenum drawmode = GL_TRIANGLES) : GLObject(geometry, material, custom_vec2_attributes, {}, drawmode)
        {
            
        }
        
        
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
