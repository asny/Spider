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
        static std::map<std::string, std::shared_ptr<GLShader>> dictionary;
        
        GLuint shader_id;
        static GLuint current_shader_id;
        
    public:
        GLShader(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "");
        
        static std::shared_ptr<GLShader> create_or_get(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "")
        {
            auto key = vertexShaderFilename + fragmentShaderFilename + geometryShaderFilename;
            auto iterator = dictionary.find(key);
            if (iterator == dictionary.end())
            {
                auto shader = std::make_shared<GLShader>(vertexShaderFilename, fragmentShaderFilename, geometryShaderFilename);
                iterator = dictionary.insert(std::make_pair(key, shader)).first;
            }
            return iterator->second;
        }
        
        void use();
        
        // ******* Vertex attribute functionality ********
        
        GLuint get_attribute_location(std::string variable_name);
        
        // ****** Uniform variable functionality ********
        
        GLuint get_uniform_location(std::string variable_name);
    };
}