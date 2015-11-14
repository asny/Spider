//
//  GLTexture.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 06/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"
#include "Bitmap.h"

namespace oogl
{
    /**
     Represents an OpenGL texture
     */
    class GLTexture
    {
    protected:
        GLuint texture_id;
        GLenum minMagFilter = GL_LINEAR; // GL_NEAREST or GL_LINEAR
        GLenum wrapMode = GL_CLAMP_TO_EDGE; // GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, or GL_CLAMP_TO_BORDER
        
        void bind_image(const tdogl::Bitmap& bitmap, GLenum target);
        
    public:
        /**
         Creates a texture from a bitmap.
         */
        GLTexture();
        
        /**
         Bind the texture and returns the id of the active texture.
         */
        virtual int use();
        
        /**
         Deletes the texture.
         */
        ~GLTexture();
    };
    
    /**
     Represents an OpenGL texture
     */
    class GLTexture2D : public GLTexture
    {
    public:
        /**
         Creates a texture from a bitmap.
         */
        GLTexture2D(const tdogl::Bitmap& bitmap);
        
        /**
         Bind the texture and returns the id of the active texture.
         */
        int use();
    };
    
    /**
     Represents an OpenGL cubemap texture
     */
    class GLTexture3D : public GLTexture
    {
    public:
        /**
         Creates a texture from a bitmap.
         */
        GLTexture3D(const tdogl::Bitmap& bitmap);
        
        /**
         Bind the texture and returns the id of the active texture.
         */
        int use();
    };

}
