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
    class GLObject {
        
        struct VertexAttribute
        {
            GLuint location;
            int size;
            
            int start_index;
            int stride_index;
        };
        
        std::shared_ptr<GLShader> shader;
        
        glm::mat4 modelMatrix = glm::mat4(1.);
        
        std::vector<float> data;
        
        GLuint buffer_id, array_id;
        
        std::map<std::string, VertexAttribute> attributes = std::map<std::string, VertexAttribute>();
        
        GLMaterial material;
        
        std::shared_ptr<GLTexture> texture;
        
        GLenum drawmode;
        
        int no_vertices = 0;
        
    public:
        
        GLObject(std::shared_ptr<GLShader> _shader, const GLMaterial& _material, GLenum _drawmode = GL_TRIANGLES, std::shared_ptr<GLTexture> _texture = nullptr);
        
        void initialize_vertex_attributes(std::vector<std::string> attribute_names, std::vector<int> attribute_sizes);
        
        void set_vertex_attribute(std::string attribute_name, const std::vector<glm::vec2>& _data);
        void set_vertex_attribute(std::string attribute_name, const std::vector<glm::vec3>& _data);
        
        void finalize_vertex_attributes();
        
        template<typename T>
        void set_uniform_variable(std::string name, const T& value)
        {
            shader->use();
            shader->set_uniform_variable(name, value);
        }
        
        void draw();
        
        void set_model_matrix(glm::mat4 _modelMatrix)
        {
            modelMatrix = _modelMatrix;
        }
    };
}
