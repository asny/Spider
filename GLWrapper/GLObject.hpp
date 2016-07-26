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
        struct VertexAttribute
        {
            std::string name;
            int size;
        };
        
        std::shared_ptr<GLShader> shader;
        std::shared_ptr<GLTexture> texture;
        GLMaterial material;
        
        // Vertex attribute data
        std::vector<float> data;
        int stride = 0;
        std::vector<VertexAttribute> attributes;
        
        // Needed for drawing
        GLuint no_vertices = 0;
        GLuint buffer_id, array_id;
        GLenum drawmode;
        bool cull_back_faces;
        static bool currently_cull_back_faces;
        
        // Transformation
        glm::mat4 modelMatrix = glm::mat4(1.);
        
        void use_vertex_attributes();
        
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
        
    public:
        
        GLObject(std::vector<VertexAttribute> _attributes, std::shared_ptr<GLShader> _shader, const GLMaterial& _material, GLenum _drawmode = GL_TRIANGLES, std::shared_ptr<GLTexture> _texture = nullptr, bool cull_back_faces = true);
        
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
            shader->set_uniform_variable(name, value);
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
