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

class Butterfly : public gle::GLNode
{
    glm::vec3 position = glm::vec3(0.f, 2.f, 0.f);
    glm::vec3 view_direction = glm::vec3(0.f, 0.f, -1.f);
    std::shared_ptr<glm::mat4> transformation = std::make_shared<glm::mat4>(1.);
    std::shared_ptr<float> wing_angle = std::make_shared<float>(0.f);
    double last_time = 0.;
    
    void update(double time);
    
    static void spawn(std::vector<std::shared_ptr<Butterfly>>& butterflies, gle::GLNode& node)
    {
        static int NO_BUTTERFLIES = 10;
        if(butterflies.size() < NO_BUTTERFLIES)
        {
            auto butterfly = std::make_shared<Butterfly>();
            butterflies.push_back(butterfly);
            node.add_child(butterfly);
        }
    }
    
    static void destroy(std::vector<std::shared_ptr<Butterfly>>& butterflies, gle::GLNode& node)
    {
        for (int i = 0; i < butterflies.size(); i++) {
            if(butterflies[i]->position.y < 0)
            {
                node.remove_child(butterflies[i]);
                butterflies.erase(butterflies.begin() + i);
                i--;
            }
        }
    }
    
public:
    Butterfly();
    
    static void spawn_and_destroy_and_update(gle::GLNode& node, double time)
    {
        static std::vector<std::shared_ptr<Butterfly>> butterflies;
        spawn(butterflies, node);
        destroy(butterflies, node);
        
        for(auto butterfly : butterflies)
        {
            butterfly->update(time);
        }
    }
};
