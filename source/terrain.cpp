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

TerrainPatch::TerrainPatch(vec2 _origo) : origo(_origo)
{
    double map_size = SIZE * Terrain::VERTICES_PER_UNIT;
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

vec3 TerrainPatch::get_surface_position_at(glm::vec3 position)
{
    vec2 parameter = vec2(position.x, position.z) - origo;
    double vertices_per_unit = static_cast<double>(Terrain::VERTICES_PER_UNIT);
    vec2 index = vec2((int)floor(parameter.x * vertices_per_unit), (int)floor(parameter.y * vertices_per_unit));
    
    assert(0 <= index.x < heightmap.size());
    assert(0 <= index.y < heightmap[0].size());
    return vec3(position.x, heightmap[index.x][index.y], position.z);
}

Terrain::Terrain()
{
    
    
}

TerrainPatch Terrain::get_or_create_patch_at(glm::vec2 parameter)
{
    pair<int, int> index = make_pair((int)floor(parameter.x / TerrainPatch::SIZE), (int)floor(parameter.y / TerrainPatch::SIZE));
    auto origo_patch_pair = terrain_patches.find(index);
    
    if (origo_patch_pair == terrain_patches.end())
    {
        auto patch = TerrainPatch(vec2(TerrainPatch::SIZE * (double)index.first, TerrainPatch::SIZE * (double)index.second));
        terrain_patches.insert(make_pair(index, patch));
        return patch;
    }
    return origo_patch_pair->second;
}

vec3 Terrain::get_terrain_position_at(const glm::vec3& position)
{
    vec2 parameter = vec2(position.x, position.z);
    return get_or_create_patch_at(parameter).get_surface_position_at(position);
}
