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
#include "Terrain.hpp"

class Butterfly : public gle::GLNode
{
    double last_time = 0.;
    double start_wing_angle;
    
    glm::mat4 rotation = glm::mat4(1.);
    glm::mat4 translation = glm::mat4(1.);
    std::shared_ptr<glm::mat4> local2world = std::make_shared<glm::mat4>(1.);
    std::shared_ptr<float> wing_angle = std::make_shared<float>(0.f);
    
    void update(Terrain& terrain);
    
    static void spawn(std::vector<std::shared_ptr<Butterfly>>& butterflies, gle::GLNode& node)
    {
        static int NO_BUTTERFLIES = 1;
        if(butterflies.size() < NO_BUTTERFLIES)
        {
            auto butterfly = std::make_shared<Butterfly>();
            butterflies.push_back(butterfly);
            node.add_child(butterfly);
        }
    }
    
    static void destroy(std::vector<std::shared_ptr<Butterfly>>& butterflies, gle::GLNode& node)
    {
//        for (int i = 0; i < butterflies.size(); i++) {
//            if(butterflies[i]->position.y < 0)
//            {
//                node.remove_child(butterflies[i]);
//                butterflies.erase(butterflies.begin() + i);
//                i--;
//            }
//        }
    }
    
public:
    Butterfly();
    
    static void spawn_and_destroy_and_update(gle::GLNode& node, Terrain& terrain)
    {
        static std::vector<std::shared_ptr<Butterfly>> butterflies;
        spawn(butterflies, node);
        destroy(butterflies, node);
        
        for(auto butterfly : butterflies)
        {
            butterfly->update(terrain);
        }
    }
};
