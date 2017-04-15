//
//  Firefly.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 15/04/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLScene.h"

class Firefly
{
    std::shared_ptr<gle::GLPointLight> point_light = std::make_shared<gle::GLPointLight>();
    
    void update()
    {
        
    }
    
    static void spawn(std::vector<std::shared_ptr<Firefly>>& fireflies, gle::GLScene& node)
    {
        static int NO_FIREFLIES = 0;
        if(fireflies.size() < NO_FIREFLIES)
        {
            auto firefly = std::make_shared<Firefly>(glm::vec3(gle::random(-10., 10.), gle::random(2., 3.), gle::random(-10., 10.)));
            fireflies.push_back(firefly);
            node.add_light(firefly->point_light);
        }
    }
    
public:
    
    Firefly(const glm::vec3& position)
    {
        point_light->position = position;
        point_light->color = glm::vec3(1., 1., 0.);
        point_light->ambient_intensity = 0.f;
        point_light->diffuse_intensity = 0.3f;
        point_light->attenuation_constant = 0.f;
        point_light->attenuation_linear = 0.1f;
        point_light->attenuation_exp = 0.1f;
    }
    
    static void spawn_and_destroy_and_update(gle::GLScene& node)
    {
        static std::vector<std::shared_ptr<Firefly>> fireflies;
        spawn(fireflies, node);
        
        for(auto firefly : fireflies)
        {
            firefly->update();
        }
    }
    
};
