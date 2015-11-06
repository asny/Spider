//
//  GLTexture.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 06/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GLTexture.hpp"

using namespace oogl;

GLTexture::GLTexture(const Bitmap& bitmap) : width((GLfloat)bitmap.get_width()), height((GLfloat)bitmap.get_height())
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 format,
                 (GLsizei)bitmap.get_width(),
                 (GLsizei)bitmap.get_height(),
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 bitmap.get_pixels());
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &texture_id);
}

GLuint GLTexture::get_id() const
{
    return texture_id;
}

GLfloat GLTexture::get_width() const
{
    return width;
}

GLfloat GLTexture::get_height() const
{
    return height;
}
