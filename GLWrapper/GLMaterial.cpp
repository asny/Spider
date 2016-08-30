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

void GLMaterial::pre_draw()
{
    shader->use();
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

void GLStandardMaterial::pre_draw()
{
    GLMaterial::pre_draw();
    ambient->use();
    diffuse->use();
    specular->use();
}

void GLTextureMaterial::pre_draw()
{
    GLMaterial::pre_draw();
    *texture_id = texture->use();
    texture_id_uniform->use();
}

void GLSkyboxMaterial::pre_draw()
{
    GLMaterial::pre_draw();
    *texture_id = texture->use();
    texture_id_uniform->use();
}

void GLGrassMaterial::pre_draw()
{
    GLMaterial::pre_draw();
    ambient->use();
    diffuse->use();
}
