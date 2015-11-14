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
        static GLuint current_shader_id;
        void use();

        struct VertexAttribute
        {
            std::string name;
            int size;
        };
        
        int stride = 0;
        std::vector<VertexAttribute> attributes;
        
    public:
        
        GLShader(std::vector<VertexAttribute> _attributes, std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "");
        
        // ******* Vertex attribute functionality ********
        
        GLuint get_attribute_location(std::string variable_name);
        
        void initialize_vertex_attributes();
        
        int get_attributes_stride()
        {
            return stride;
        }
        
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
        
        void use_vertex_attributes();
        
        // ****** Uniform variable functionality ********
        
        GLuint get_uniform_location(std::string variable_name);
        GLuint get_uniform_location_with_warning(std::string variable_name);
        
        void set_uniform_variable_if_defined(std::string name, const glm::mat4& value);
        
        void set_uniform_variable(std::string name, int value);
        void set_uniform_variable(std::string name, const glm::vec2& value);
        void set_uniform_variable(std::string name, const glm::vec3& value);
        void set_uniform_variable(std::string name, const glm::vec4& value);
        void set_uniform_variable(std::string name, const glm::mat4& value);
    };
}