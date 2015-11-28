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

TerrainPatch::TerrainPatch(const vec3& _origo, double size) : origo(_origo)
{
    int map_size = static_cast<int>(size) * VERTICES_PER_UNIT + 1;
    heightmap = vector<vector<double>>(map_size);
    grass = vector<vector<vec3>>(map_size);
    for ( auto r = 0; r < map_size; r++ )
    {
        heightmap[r] = vector<double>(map_size);
        grass[r] = vector<vec3>(map_size);
    }
    
    double scale = (map_size - 1) / static_cast<double>(VERTICES_PER_UNIT);
    set_height(scale, 0, 0, {});
    set_height(scale, 0, map_size - 1, {});
    set_height(scale, map_size - 1, 0, {});
    set_height(scale, map_size - 1, map_size - 1, {});
    
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
    vec3 position = vec3(origo.x + r/static_cast<double>(VERTICES_PER_UNIT), 0., origo.z + c/static_cast<double>(VERTICES_PER_UNIT));
    heightmap[r][c] = average(neighbour_heights) + 0.15 * scale * static_cast<double>(raw_noise_2d(position.x, position.z));
    grass[r][c] = vec3(static_cast<double>(0.5 * raw_noise_3d(position.x + 10., position.z + 10., clock())),
                       static_cast<double>(0.5 * raw_noise_3d(position.x, position.z, clock()) + 1.),
                       static_cast<double>(0.5 * raw_noise_3d(position.x - 10., position.z - 10., clock())));
}

void TerrainPatch::subdivide(int origo_r, int origo_c, int size)
{
    int half_size = size/2;
    if(half_size >= 1)
    {
        double scale = size / static_cast<double>(VERTICES_PER_UNIT);
        
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

vec2 TerrainPatch::index_at(const vec3& position) const
{
    double vertices_per_unit = static_cast<double>(VERTICES_PER_UNIT);
    vec2 index = vec2((int)floor((position.x - origo.x) * vertices_per_unit), (int)floor((position.z - origo.z) * vertices_per_unit));
    
    assert(0 <= index.x < heightmap.size());
    assert(0 <= index.y < heightmap[0].size());
    return index;
}

double TerrainPatch::get_surface_height_at(const vec3& position) const
{
    vec2 index = index_at(position);
    return heightmap[index.x][index.y];
}

vec3 TerrainPatch::get_grass_vector_at(const vec3& position) const
{
    vec2 index = index_at(position);
    return grass[index.x][index.y];
}

vec3 TerrainPatch::get_origo()
{
    return origo;
}

vec3 TerrainPatch::get_size()
{
    return vec3(static_cast<double>(heightmap.size() / VERTICES_PER_UNIT), 0., static_cast<double>(heightmap[0].size() / VERTICES_PER_UNIT));
}

Terrain::Terrain()
{
    
    
}

pair<int, int> Terrain::index_at(const vec3& position)
{
    vec2 parameter = vec2(position.x, position.z);
    vec2 index_vector = vec2(floor(parameter.x / patch_size), floor(parameter.y / patch_size));
    return make_pair(static_cast<int>(index_vector.x), static_cast<int>(index_vector.y));
}

TerrainPatch* Terrain::create_patch_at(pair<int, int> index)
{
    vec3 origo = vec3(patch_size * static_cast<double>(index.first), 0., patch_size * static_cast<double>(index.second));
    TerrainPatch patch = TerrainPatch(origo, patch_size);
    return &terrain_patches.insert(make_pair(index, patch)).first->second;
}

TerrainPatch* Terrain::get_patch_at(std::pair<int, int> index)
{
    auto index_patch_pair = terrain_patches.find(index);
    
    // If the patch hasn't been created, then we create it.
    if (index_patch_pair == terrain_patches.end())
    {
        return create_patch_at(index);
    }
    return &index_patch_pair->second;
}

vec3 Terrain::get_terrain_position_at(const glm::vec3& position)
{
    auto index = index_at(position);
    TerrainPatch* patch = get_patch_at(index);
    double height = patch->get_surface_height_at(position);
    return vec3(position.x, height, position.z);
}

vec3 Terrain::get_grass_vector_at(const glm::vec3& position)
{
    auto index = index_at(position);
    TerrainPatch* patch = get_patch_at(index);
    return patch->get_grass_vector_at(position);
}
