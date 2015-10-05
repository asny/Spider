//
//  model.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "model.hpp"

std::vector<glm::vec3> Model::get_terrain()
{
    glm::vec3 pos = spider.get_position();
    TerrainPatch patch = terrain.get_patch_at(0,0);
    
    int radius = 10;
    glm::vec2 p;
    std::vector<glm::vec3> terrain;
    for(int y = pos.y - radius; y < pos.y + radius; y++)
    {
        for(int x = pos.x - radius; x < pos.x + radius; x++)
        {
            p = glm::vec2(x,y);
            terrain.push_back(glm::vec3(p.x, patch.get_height_at(p), p.y));
            
            p = glm::vec2(x,y + 1);
            terrain.push_back(glm::vec3(p.x, patch.get_height_at(p), p.y));
        }
    }
    
    return terrain;
}
