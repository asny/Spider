//
//  Random.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/02/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "simplexnoise.h"

class Random {
    
    
public:
    static double value(double min, double max)
    {
        return (max - min) * (double)rand()/(double)RAND_MAX + min;
    }
    
    static glm::vec3 direction()
    {
        double theta = value(0., 2. * M_PI);
        double phi = value(0., M_PI);
        double sin_phi = sin(phi);
        return glm::vec3(cos(theta) * sin_phi, cos(phi), sin(theta) * sin_phi);
    }
    
};
