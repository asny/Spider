//
//  model.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "model.hpp"
#include "glm.hpp"
#include "simplexnoise.h"

using namespace std;
using namespace glm;

vector<TerrainPatch*> Model::terrain_patches_to_update()
{
    vec3 spider_pos = spider.get_position();
    pair<int, int> center = terrain.index_at(spider_pos);
    auto to_update = vector<TerrainPatch*>();
    
    if (terrainIndexMap.size() == 0) {
        for (int i = 0; i < 9; i++) {
            terrainIndexMap[i] = {center.first - 1 + i % 3, center.second - 1 + std::floor(i/3)};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
        return to_update;
    }
    
    for (int i = 0; i < 9; i++)
    {
        auto current = terrainIndexMap[i];
        if(center.first - current.first == 2 && center.second - current.second == 2)
        {
            terrainIndexMap[i] = {center.first + 1, center.second + 1};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
        else if(center.first - current.first == -2 && center.second - current.second == -2)
        {
            terrainIndexMap[i] = {center.first - 1, center.second - 1};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
        else if(center.first - current.first == 2 && center.second - current.second == -2)
        {
            terrainIndexMap[i] = {center.first + 1, center.second - 1};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
        else if(center.first - current.first == -2 && center.second - current.second == 2)
        {
            terrainIndexMap[i] = {center.first - 1, center.second + 1};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
        else if(center.first - current.first == 2)
        {
            terrainIndexMap[i] = {center.first + 1, current.second};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
        else if(center.first - current.first == -2)
        {
            terrainIndexMap[i] = {center.first - 1, current.second};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
        else if(center.second - current.second == 2)
        {
            terrainIndexMap[i] = {current.first, center.second + 1};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
        else if(center.second - current.second == -2)
        {
            terrainIndexMap[i] = {current.first, center.second - 1};
            to_update.push_back(terrain.get_patch_at(terrainIndexMap[i]));
        }
    }
    
    return to_update;
}
