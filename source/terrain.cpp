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

const double patch_size = 8.;
const double patch_discretization = 8.;
const int map_size = patch_size * patch_discretization;

TerrainPatch::TerrainPatch(vec2 _origo) : origo(_origo)
{
    heightmap = vector<vector<double>>(map_size+1);
    for ( auto r = 0; r < map_size+1; r++ )
    {
        heightmap[r] = vector<double>(map_size+1);
    }
    heightmap[0][0] = -0.5;
    heightmap[0][map_size] = 0.5;
    heightmap[map_size][0] = 1.;
    heightmap[map_size][map_size] = 2.;
    
    subdivide(0, 0, map_size);
}

double average(std::vector<double> heights)
{
    if(heights.size() == 0)
    {
        return 0.;
    }
    double sum = 0.;
    for (auto height : heights)
    {
        sum += height;
    }
    return sum / static_cast<double>(heights.size());
}

void TerrainPatch::set_height(int r, int c, std::vector<double> neighbour_heights)
{
    
    heightmap[r][c] = average(neighbour_heights) + 0.05*(double)raw_noise_2d(r, c);
}

void TerrainPatch::subdivide(int origo_r, int origo_c, int size)
{
    int half_size = size/2;
    if(half_size >= 1)
    {
        set_height(origo_r + half_size, origo_c, {heightmap[origo_r][origo_c], heightmap[origo_r + size][origo_c]});
        set_height(origo_r, origo_c + half_size, {heightmap[origo_r][origo_c], heightmap[origo_r][origo_c + size]});
        set_height(origo_r + half_size, origo_c + size, {heightmap[origo_r + size][origo_c + size], heightmap[origo_r][origo_c + size]});
        set_height(origo_r + size, origo_c + half_size, {heightmap[origo_r + size][origo_c + size], heightmap[origo_r + size][origo_c]});
        set_height(origo_r + half_size, origo_c + half_size, {heightmap[origo_r + half_size][origo_c], heightmap[origo_r][origo_c + half_size],
            heightmap[origo_r + half_size][origo_c + size], heightmap[origo_r + size][origo_c + half_size]});
        
        subdivide(origo_r, origo_c, half_size);
        subdivide(origo_r + half_size, origo_c, half_size);
        subdivide(origo_r, origo_c + half_size, half_size);
        subdivide(origo_r + half_size, origo_c + half_size, half_size);
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
