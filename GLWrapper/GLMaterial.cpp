//
//  GLMaterial.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 27/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#include "GLMaterial.hpp"

using namespace oogl;
using namespace std;
using namespace glm;

bool GLMaterial::currently_cull_back_faces = true;

void GLMaterial::PreDrawing()
{
    if(currently_cull_back_faces != cull_back_faces)
    {
        if(cull_back_faces)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        currently_cull_back_faces = cull_back_faces;
    }
}

void GLStandardMaterial::PreDrawing()
{
    GLMaterial::PreDrawing();
    shader->set_uniform_variable("ambientMat", ambient);
    shader->set_uniform_variable("diffuseMat", diffuse);
    shader->set_uniform_variable("specMat", specular);
}

void GLTextureMaterial::PreDrawing()
{
    GLMaterial::PreDrawing();
    int texture_id = texture->use();
    shader->set_uniform_variable("texture0", texture_id);
}

void GLSkyboxMaterial::PreDrawing()
{
    GLMaterial::PreDrawing();
    int texture_id = texture->use();
    shader->set_uniform_variable("texture0", texture_id);
}

void GLGrassMaterial::PreDrawing()
{
    GLMaterial::PreDrawing();
    shader->set_uniform_variable("ambientMat", ambient);
    shader->set_uniform_variable("diffuseMat", diffuse);
}
