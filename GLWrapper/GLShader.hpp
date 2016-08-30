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
        
    public:
        
        GLShader(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename = "");
        
        void use();
        
        // ******* Vertex attribute functionality ********
        
        GLuint get_attribute_location(std::string variable_name);
        
        // ****** Uniform variable functionality ********
        
        GLuint get_uniform_location(std::string variable_name);
    };
}