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
const double patch_discretization = 1.;
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

vector<vector<vec3>> TerrainPatch::get_data()
{
    auto data = vector<vector<vec3>>(map_size);
    for ( int r = 0; r < map_size; r++ )
    {
        data[r] = vector<vec3>(map_size);
        for ( int c = 0; c < map_size; c++ )
        {
            vec2 pos = vec2(origo.x + static_cast<double>(c)/patch_discretization, origo.y + static_cast<double>(r)/patch_discretization);
            data[r][c] = vec3(pos.x, heightmap[r][c], -pos.y);
        }
    }
    return data;
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

vector<vector<vec3>> Terrain::get_patch_at(glm::vec3 position)
{
    pair<int, int> index = make_pair((int)floor(position.x / patch_size), (int)floor(-position.z / patch_size));
    auto origo_patch_pair = terrain_patches.find(index);
    
    if (origo_patch_pair == terrain_patches.end())
    {
        auto patch = TerrainPatch(vec2((double)index.first, (double)index.second));
        terrain_patches.insert(make_pair(index, patch));
        return patch.get_data();
    }
    return origo_patch_pair->second.get_data();
}
