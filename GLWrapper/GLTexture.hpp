//
//  GLTexture.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 06/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <OpenGL/gl3.h>

namespace oogl
{
    class Bitmap
    {
    public:
        const GLvoid* get_pixels() const
        {
            return 0;
        }
        
        int get_width() const
        {
            return 0;
        }
        
        int get_height() const
        {
            return 0;
        }
    };
    
    /**
     Represents an OpenGL texture
     */
    class GLTexture
    {
        GLuint texture_id;
        GLfloat width;
        GLfloat height;
        GLenum format = GL_RGB;
        GLenum minMagFilter = GL_LINEAR; // GL_NEAREST or GL_LINEAR
        GLenum wrapMode = GL_CLAMP_TO_EDGE; // GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, or GL_CLAMP_TO_BORDER
        
    public:
        /**
         Creates a texture from a bitmap.
         */
        GLTexture(const Bitmap& bitmap);
        
        /**
         Deletes the texture
         */
        ~GLTexture();
        
        /**
         Returns the texture id.
         */
        GLuint get_id() const;
        
        /**
         Returns the width
         */
        GLfloat get_width() const;
        
        /**
         Returns the height
         */
        GLfloat get_height() const;
    };

}
