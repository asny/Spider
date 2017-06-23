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
#include "Spider.hpp"

class Butterfly : public gle::GLNode
{
    double last_time = gle::time();
    double start_wing_angle;
    
    glm::mat4 rotation = glm::mat4(1.);
    glm::mat4 translation = glm::mat4(1.);
    std::shared_ptr<glm::mat4> local2world = std::make_shared<glm::mat4>(1.);
    std::shared_ptr<float> wing_angle = std::make_shared<float>(0.f);
    
    void update(Terrain& terrain, const Spider& spider);
    
    static void spawn(std::vector<std::shared_ptr<Butterfly>>& butterflies, gle::GLNode& node, const glm::vec3& spider_position)
    {
        static int NO_BUTTERFLIES = 100;
        if(butterflies.size() < NO_BUTTERFLIES)
        {
            auto butterfly = std::make_shared<Butterfly>(spider_position);
            butterflies.push_back(butterfly);
            node.add_child(butterfly);
        }
    }
    
    static void destroy(std::vector<std::shared_ptr<Butterfly>>& butterflies, gle::GLNode& node, const glm::vec3& spider_position, Terrain& terrain)
    {
        for (int i = 0; i < butterflies.size(); i++)
        {
            auto current_position = glm::vec3(butterflies[i]->translation[3]);
            if(!terrain.is_inside(current_position))
            {
                node.remove_child(butterflies[i]);
                butterflies.erase(butterflies.begin() + i);
                i--;
            }
        }
    }
    
public:
    Butterfly(const glm::vec3& spider_position);
    
    static void spawn_and_destroy_and_update(gle::GLNode& node, Terrain& terrain, const Spider& spider)
    {
        glm::vec3 spider_position = spider.get_position(terrain);
        static std::vector<std::shared_ptr<Butterfly>> butterflies;
        spawn(butterflies, node, spider_position);
        destroy(butterflies, node, spider_position, terrain);
        
        for(auto butterfly : butterflies)
        {
            butterfly->update(terrain, spider);
        }
    }
};
