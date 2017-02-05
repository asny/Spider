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
    std::shared_ptr<float> butterfly_angle = std::make_shared<float>(0.f);
    
public:
    Butterfly(gle::GLNode& scene);
    
    void update(double time);
};
