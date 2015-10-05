//
//  model.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "model.hpp"

using namespace glm;

std::vector<glm::vec3> Model::get_terrain()
{
    glm::vec3 pos = spider.get_position();
    TerrainPatch patch = terrain.get_patch_at(0,0);
    
    int radius = 10;
    vec2 origo = vec2((int)pos.x - radius, (int)pos.y - radius);
    vec2 size = vec2((int)pos.x + radius, (int)pos.y + radius);
    
    glm::vec2 p;
    std::vector<glm::vec3> terrain;
    for(int y = origo.y; y < size.y - 1; y++)
    {
        for(int x = origo.x; x < size.x; x++)
        {
            p = glm::vec2(x,y);
            terrain.push_back(glm::vec3(p.y, patch.get_height_at(p), p.x));
            
            if (x == origo.x)
            {
                terrain.push_back(glm::vec3(p.y, patch.get_height_at(p), p.x));
            }
            
            p = glm::vec2(x,y + 1);
            terrain.push_back(glm::vec3(p.y, patch.get_height_at(p), p.x));
            
            if (x == size.x - 1)
            {
                terrain.push_back(glm::vec3(p.y, patch.get_height_at(p), p.x));
            }
        }
    }
    
    return terrain;
}
