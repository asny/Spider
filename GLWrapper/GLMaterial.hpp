//
//  GLMaterial.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 27/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"
#include "GLShader.hpp"
#include "GLTexture.hpp"

namespace oogl
{
    class GLMaterial
    {
    public:
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        
        
    };
}
