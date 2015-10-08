//
//  model.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "model.hpp"

using namespace std;
using namespace glm;

std::vector<glm::vec3> Model::get_terrain()
{
    vec3 pos = spider.get_position();
    vector<vector<vec3>> patch = terrain.get_patch_at(pos);
    
    vector<vec3> terrain;
    for(int r = 0; r < patch.size() - 1; r++)
    {
        for(int c = 0; c < patch[0].size(); c++)
        {
            terrain.push_back(patch[r][c]);
            
            if (c == 0)
            {
                terrain.push_back(patch[r][c]);
            }
            
            terrain.push_back(patch[r+1][c]);
            
            if (c == patch[0].size() - 1)
            {
                terrain.push_back(patch[r+1][c]);
            }
        }
    }
    
    return terrain;
}
