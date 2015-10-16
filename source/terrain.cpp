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

TerrainPatch::TerrainPatch(vec2 _origo, double size) : origo(_origo)
{
    int map_size = static_cast<int>(size) * Terrain::VERTICES_PER_UNIT + 1;
    heightmap = vector<vector<double>>(map_size);
    for ( auto r = 0; r < map_size; r++ )
    {
        heightmap[r] = vector<double>(map_size);
    }
    heightmap.front().front() = 0.;
    heightmap.front().back() = 0.;
    heightmap.back().front() = 0.;
    heightmap.back().back() = 0.;
    
    subdivide(0, 0, map_size-1);
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

void TerrainPatch::set_height(double scale, int r, int c, std::vector<double> neighbour_heights)
{
    heightmap[r][c] = average(neighbour_heights) + 0.3 * scale * static_cast<double>(octave_noise_2d(3.f, 0.25f, scale, r, c));
}

void TerrainPatch::subdivide(int origo_r, int origo_c, int size)
{
    int half_size = size/2;
    if(half_size >= 1)
    {
        double scale = size / static_cast<double>(Terrain::VERTICES_PER_UNIT);
        
        set_height(scale, origo_r + half_size, origo_c, {heightmap[origo_r][origo_c], heightmap[origo_r + size][origo_c]});
        set_height(scale, origo_r, origo_c + half_size, {heightmap[origo_r][origo_c], heightmap[origo_r][origo_c + size]});
        set_height(scale, origo_r + half_size, origo_c + size, {heightmap[origo_r + size][origo_c + size], heightmap[origo_r][origo_c + size]});
        set_height(scale, origo_r + size, origo_c + half_size, {heightmap[origo_r + size][origo_c + size], heightmap[origo_r + size][origo_c]});
        set_height(scale, origo_r + half_size, origo_c + half_size, {heightmap[origo_r + half_size][origo_c], heightmap[origo_r][origo_c + half_size],
            heightmap[origo_r + half_size][origo_c + size], heightmap[origo_r + size][origo_c + half_size]});
        
        subdivide(origo_r, origo_c, half_size);
        subdivide(origo_r + half_size, origo_c, half_size);
        subdivide(origo_r, origo_c + half_size, half_size);
        subdivide(origo_r + half_size, origo_c + half_size, half_size);
    }
}

double TerrainPatch::get_surface_height_at(glm::vec2 parameter)
{
    double vertices_per_unit = static_cast<double>(Terrain::VERTICES_PER_UNIT);
    vec2 index = vec2((int)floor((parameter.x - origo.x) * vertices_per_unit), (int)floor((parameter.y - origo.y) * vertices_per_unit));
    
    assert(0 <= index.x < heightmap.size());
    assert(0 <= index.y < heightmap[0].size());
    return heightmap[index.x][index.y];
}

Terrain::Terrain()
{
    
    
}

TerrainPatch Terrain::get_or_create_patch_at(glm::vec2 parameter)
{
    const double patch_size = 8.;
    vec2 index_vector = vec2(floor(parameter.x / patch_size), floor(parameter.y / patch_size));
    pair<int, int> index = make_pair(static_cast<int>(index_vector.x), static_cast<int>(index_vector.y));
    auto index_patch_pair = terrain_patches.find(index);
    
    // If the patch hasn't been created, then we create it.
    if (index_patch_pair == terrain_patches.end())
    {
        vec2 origo = vec2(patch_size * index_vector.x, patch_size * index_vector.y);
        TerrainPatch patch = TerrainPatch(origo, patch_size);
        terrain_patches.insert(make_pair(index, patch));
        return patch;
    }
    return index_patch_pair->second;
}

vec3 Terrain::get_terrain_position_at(const glm::vec3& position)
{
    vec2 parameter = vec2(position.x, position.z);
    double height = get_or_create_patch_at(parameter).get_surface_height_at(parameter);
    return vec3(position.x, height, position.z);
}
