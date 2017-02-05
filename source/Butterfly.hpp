//
//  Butterfly.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/02/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <memory>
#include "GLScene.h"

class Butterfly
{
    glm::vec3 position = glm::vec3(0.f, 2.f, 0.f);
    glm::vec3 view_direction = glm::vec3(0.f, 0.f, -1.f);
    std::shared_ptr<glm::mat4> transformation = std::make_shared<glm::mat4>(1.);
    std::shared_ptr<float> wing_angle = std::make_shared<float>(0.f);
    
public:
    Butterfly(gle::GLNode& scene);
    
    void update(double time);
};
