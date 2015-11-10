//
//  GLTexture.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 06/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"
#include "GLShader.hpp"
#include "Bitmap.h"

namespace oogl
{
    /**
     Represents an OpenGL texture
     */
    class GLTexture
    {
        GLuint texture_id;
        GLfloat width;
        GLfloat height;
        GLenum minMagFilter = GL_LINEAR; // GL_NEAREST or GL_LINEAR
        GLenum wrapMode = GL_CLAMP_TO_EDGE; // GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, or GL_CLAMP_TO_BORDER
        
        const std::shared_ptr<GLShader> shader;
        
    public:
        /**
         Creates a texture from a bitmap.
         */
        GLTexture(const std::shared_ptr<GLShader> _shader, const tdogl::Bitmap& bitmap);
        
        /**
         Bind the texture and set the "tex" uniform in the fragment shader
         */
        void use();
        
        /**
         Deletes the texture.
         */
        ~GLTexture();
    };

}
