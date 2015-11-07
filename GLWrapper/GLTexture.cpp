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

GLTexture::GLTexture(const std::shared_ptr<GLShader> _shader, const tdogl::Bitmap& bitmap, const std::vector<glm::vec2>& _uv_coordinates) :
    shader(_shader), uv_coordinates(_uv_coordinates)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
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
}

void GLTexture::use()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    shader->set_uniform_variable("texture0", 0); //set to 0 because the texture is bound to GL_TEXTURE0
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &texture_id);
}
