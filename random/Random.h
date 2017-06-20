//
//  Random.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/02/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "simplexnoise.h"
#include "gtx/rotate_vector.hpp"

class Random {
    
    
public:
    template<typename T>
    static T value(T min, T max)
    {
        return (max - min) * (T)rand()/(T)RAND_MAX + min;
    }
    
    static glm::vec3 direction()
    {
        double theta = value(0., 2. * M_PI);
        double phi = value(0., M_PI);
        double sin_phi = sin(phi);
        return glm::vec3(cos(theta) * sin_phi, cos(phi), sin(theta) * sin_phi);
    }
    
    static glm::vec3 hemisphere_direction(const glm::vec3& direction)
    {
        double theta = value(0., 2. * M_PI);
        double phi = value(0., 0.5 * M_PI);
        double sin_phi = sin(phi);
        return glm::mat3(glm::orientation(direction, glm::vec3(0., 1., 0.))) * glm::vec3(cos(theta) * sin_phi, cos(phi), sin(theta) * sin_phi);
    }
    
};
