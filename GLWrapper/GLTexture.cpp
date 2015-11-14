//
//  GLTexture.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 06/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GLTexture.hpp"

using namespace oogl;

static GLenum TextureFormatForBitmapFormat(tdogl::Bitmap::Format format)
{
    switch (format) {
        case tdogl::Bitmap::Format_RGB: return GL_RGB;
        case tdogl::Bitmap::Format_RGBA: return GL_RGBA;
        default: throw std::runtime_error("Unrecognised Bitmap::Format");
    }
}

GLTexture::GLTexture()
{
    glGenTextures(1, &texture_id);
    check_gl_error();
}

int GLTexture::use()
{
    glActiveTexture(GL_TEXTURE0);
    check_gl_error();
    return 0; //return 0 because the texture is bound to GL_TEXTURE0
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &texture_id);
    check_gl_error();
}

GLTexture2D::GLTexture2D(const tdogl::Bitmap& bitmap) : GLTexture::GLTexture()
{
    use();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 TextureFormatForBitmapFormat(bitmap.format()),
                 (GLsizei)bitmap.width(),
                 (GLsizei)bitmap.height(),
                 0,
                 TextureFormatForBitmapFormat(bitmap.format()),
                 GL_UNSIGNED_BYTE,
                 bitmap.pixelBuffer());
    glBindTexture(GL_TEXTURE_2D, 0);
    check_gl_error();
}

int GLTexture2D::use()
{
    int id = GLTexture::use();
    glBindTexture(GL_TEXTURE_2D, texture_id);
    check_gl_error();
    return id;
}

GLTexture3D::GLTexture3D(const tdogl::Bitmap& bitmap) : GLTexture::GLTexture()
{
    use();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapMode);
    glTexImage2D(GL_TEXTURE_CUBE_MAP,
                 0,
                 TextureFormatForBitmapFormat(bitmap.format()),
                 (GLsizei)bitmap.width(),
                 (GLsizei)bitmap.height(),
                 0,
                 TextureFormatForBitmapFormat(bitmap.format()),
                 GL_UNSIGNED_BYTE,
                 bitmap.pixelBuffer());
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    check_gl_error();
}

int GLTexture3D::use()
{
    int id = GLTexture::use();
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    check_gl_error();
    return id;
}
