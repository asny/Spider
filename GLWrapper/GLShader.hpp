//
//  GLShader.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 07/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"

namespace oogl {
    /**
     Represents a shader used to draw a GLObject.
     */
    class GLShader
    {
        GLuint shader_id;

        struct VertexAttribute
        {
            int size;
            int start_index;
        };
        
        int stride = 0;
        int current_start_index = 0;
        std::map<std::string, VertexAttribute> attributes = std::map<std::string, VertexAttribute>();
        
        GLuint get_uniform_location(std::string variable_name);
        GLuint get_uniform_location_with_warning(std::string variable_name);
        
        void set_uniform_variable_if_defined(std::string name, const glm::mat4& value);
        
    public:
        static glm::mat4 viewMatrix;
        static glm::mat4 projectionMatrix;
        
        GLShader(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "");
        
        void use();
        
        // Vertex attribute functionality
        int get_attributes_stride()
        {
            return stride;
        }
        
        int get_attribute_start_index(std::string name)
        {
            return attributes.find(name)->second.start_index;
        }
        
        void initialize_vertex_attributes()
        {
            for (auto attribute_names : attributes)
            {
                GLuint location = get_attribute_location(attribute_names.first);
                glEnableVertexAttribArray(location);
            }
        }
        
        void create_vertex_attribute(std::string name, int size);
        
        void use_vertex_attributes()
        {
            for (auto attribute_name_value_pair : attributes)
            {
                auto attribute = attribute_name_value_pair.second;
                GLuint location = get_attribute_location(attribute_name_value_pair.first);
                glVertexAttribPointer(location, attribute.size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const GLvoid *)(attribute.start_index * sizeof(float)));
            }
        }
        
        // Uniform variable functionality
        GLuint get_attribute_location(std::string variable_name);
        
        void set_uniform_variable(std::string name, int value);
        void set_uniform_variable(std::string name, const glm::vec2& value);
        void set_uniform_variable(std::string name, const glm::vec3& value);
        void set_uniform_variable(std::string name, const glm::vec4& value);
        void set_uniform_variable(std::string name, const glm::mat4& value);
        
        // Updates the draw matrices
        void update_draw_matrices(const glm::mat4& modelMatrix);
    };
}