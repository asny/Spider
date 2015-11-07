//
//  GLTexture.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 06/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GLTexture.hpp"

using namespace oogl;

GLTexture::GLTexture(const std::shared_ptr<GLShader> _shader, const tdogl::Bitmap& bitmap, const std::vector<glm::vec2>& uv_coordinates) : shader(_shader)
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
                 (GLsizei)bitmap.width(),
                 (GLsizei)bitmap.height(),
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 bitmap.pixelBuffer());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture::use()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    shader->set_uniform_variable("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &texture_id);
}
