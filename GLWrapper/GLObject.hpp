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
    class GLObject
    {
        std::shared_ptr<GLShader> shader;
        std::shared_ptr<GLTexture> texture;
        GLMaterial material;
        
        // Vertex attribute data
        std::vector<float> data;
        
        // Needed for drawing
        GLuint no_vertices = 0;
        GLuint buffer_id, array_id;
        GLenum drawmode;
        
        // Transformations
        glm::mat4 modelMatrix = glm::mat4(1.);
        static glm::mat4 viewMatrix;
        static glm::mat4 projectionMatrix;
        
    public:
        
        GLObject(std::shared_ptr<GLShader> _shader, const GLMaterial& _material, GLenum _drawmode = GL_TRIANGLES, std::shared_ptr<GLTexture> _texture = nullptr);
        
        void set_vertex_attribute(std::string name, const std::vector<glm::vec2>& _data);
        void set_vertex_attribute(std::string name, const std::vector<glm::vec3>& _data);
        
        void finalize_vertex_attributes();
        
        template<typename T>
        void set_uniform_variable(std::string name, const T& value)
        {
            shader->use();
            shader->set_uniform_variable(name, value);
        }
        
        void draw();
        
        // Setters for the transformations
        void set_model_matrix(glm::mat4 _modelMatrix)
        {
            modelMatrix = _modelMatrix;
        }
        
        static void set_view_matrix(glm::mat4 _viewMatrix)
        {
            viewMatrix = _viewMatrix;
        }
        
        static void set_projection_matrix(glm::mat4 _projectionMatrix)
        {
            projectionMatrix = _projectionMatrix;
        }
    };
}
