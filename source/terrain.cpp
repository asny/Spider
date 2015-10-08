//
//  terrain.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "terrain.hpp"
#include "simplexnoise.h"

using namespace std;
using namespace glm;

const double patch_size = 100.;
const int patch_discretization = 2;
const int map_size = patch_size * patch_discretization;

TerrainPatch::TerrainPatch(vec2 _origo) : origo(_origo)
{
    heightmap = vector<vector<double>>(map_size);
    for ( auto r = 0; r < map_size; r++ )
    {
        heightmap[r] = vector<double>(map_size);
        for ( auto c = 0; c < map_size; c++ )
        {
            heightmap[r][c] = (double)raw_noise_2d(r, c);
        }
    }
}


double TerrainPatch::get_height_at(glm::vec2 position)
{
    vec2 index = (position - origo) * (float)patch_discretization;
    assert(0 <= index.x <= map_size);
    assert(0 <= index.y <= map_size);
    return heightmap[floor(index.x)][floor(index.y)];
}


Terrain::Terrain()
{
    
    
}

TerrainPatch Terrain::get_patch_at(glm::vec2 position)
{
    pair<int, int> index = make_pair((int)floor(position.x / patch_size), (int)floor(position.y / patch_size));
    auto origo_patch_pair = terrain_patches.find(index);
    
    if (origo_patch_pair == terrain_patches.end())
    {
        auto patch = TerrainPatch(vec2((double)index.first, (double)index.second));
        terrain_patches.insert(make_pair(index, patch));
        return patch;
    }
    return origo_patch_pair->second;
}
