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

TerrainPatch::TerrainPatch(vec2 _origo, double size, vector<const TerrainPatch*> _neighbour_patches) : origo(_origo), neighbour_patches(_neighbour_patches)
{
    int map_size = static_cast<int>(size) * Terrain::VERTICES_PER_UNIT + 1;
    heightmap = vector<vector<double>>(map_size);
    grass = vector<vector<vec3>>(map_size);
    for ( auto r = 0; r < map_size; r++ )
    {
        heightmap[r] = vector<double>(map_size);
        grass[r] = vector<vec3>(map_size);
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
    vec2 parameter = origo + vec2(r/static_cast<double>(Terrain::VERTICES_PER_UNIT),c/static_cast<double>(Terrain::VERTICES_PER_UNIT));
    if(r == 0 && neighbour_patches[NORTH])
    {
        heightmap[r][c] = neighbour_patches[NORTH]->get_surface_height_at(parameter);
        grass[r][c] = neighbour_patches[NORTH]->get_grass_vector_at(parameter);
    }
    else if(c == 0 && neighbour_patches[WEST])
    {
        heightmap[r][c] = neighbour_patches[WEST]->get_surface_height_at(parameter);
        grass[r][c] = neighbour_patches[WEST]->get_grass_vector_at(parameter);
    }
    else if(r == heightmap.size() && neighbour_patches[SOUTH])
    {
        heightmap[r][c] = neighbour_patches[SOUTH]->get_surface_height_at(parameter);
        grass[r][c] = neighbour_patches[SOUTH]->get_grass_vector_at(parameter);
    }
    else if(c == heightmap[0].size() && neighbour_patches[EAST])
    {
        heightmap[r][c] = neighbour_patches[EAST]->get_surface_height_at(parameter);
        grass[r][c] = neighbour_patches[EAST]->get_grass_vector_at(parameter);
    }
    else
    {
        heightmap[r][c] = average(neighbour_heights) + 0.3 * scale * static_cast<double>(octave_noise_2d(3.f, 0.25f, scale, r, c));
        
        if(rand() % 2 == 0)
        {
            grass[r][c] = vec3(static_cast<double>(scaled_octave_noise_2d(3.f, 0.25f, scale, -1., 1., r, c)),
                               static_cast<double>(scaled_octave_noise_2d(3.f, 0.25f, scale, 0.25, 1.5, r, c)),
                               static_cast<double>(scaled_octave_noise_2d(3.f, 0.25f, scale, -1., 1., r, c)));
        }
        else {
            grass[r][c] = vec3(0.);
        }
    }
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

vec2 TerrainPatch::index_at(glm::vec2 parameter) const
{
    double vertices_per_unit = static_cast<double>(Terrain::VERTICES_PER_UNIT);
    vec2 index = vec2((int)floor((parameter.x - origo.x) * vertices_per_unit), (int)floor((parameter.y - origo.y) * vertices_per_unit));
    
    assert(0 <= index.x < heightmap.size());
    assert(0 <= index.y < heightmap[0].size());
    return index;
}

double TerrainPatch::get_surface_height_at(glm::vec2 parameter) const
{
    vec2 index = index_at(parameter);
    return heightmap[index.x][index.y];
}

vec3 TerrainPatch::get_grass_vector_at(vec2 parameter) const
{
    vec2 index = index_at(parameter);
    return grass[index.x][index.y];
}

Terrain::Terrain()
{
    
    
}

pair<int, int> Terrain::index_at(vec2 parameter)
{
    vec2 index_vector = vec2(floor(parameter.x / patch_size), floor(parameter.y / patch_size));
    return make_pair(static_cast<int>(index_vector.x), static_cast<int>(index_vector.y));
}

TerrainPatch* Terrain::create_patch_at(pair<int, int> index)
{
    vec2 origo = vec2(patch_size * static_cast<double>(index.first), patch_size * static_cast<double>(index.second));
    
    vector<const TerrainPatch*> neighbour_patches(4);
    auto index_patch_pair = terrain_patches.find(make_pair(index.first - 1, index.second));
    neighbour_patches[TerrainPatch::NORTH] = index_patch_pair != terrain_patches.end() ? &index_patch_pair->second : nullptr;
    
    index_patch_pair = terrain_patches.find(make_pair(index.first + 1, index.second));
    neighbour_patches[TerrainPatch::SOUTH] = index_patch_pair != terrain_patches.end() ? &index_patch_pair->second : nullptr;
    
    index_patch_pair = terrain_patches.find(make_pair(index.first, index.second - 1));
    neighbour_patches[TerrainPatch::WEST] = index_patch_pair != terrain_patches.end() ? &index_patch_pair->second : nullptr;
    
    index_patch_pair = terrain_patches.find(make_pair(index.first, index.second + 1));
    neighbour_patches[TerrainPatch::EAST] = index_patch_pair != terrain_patches.end() ? &index_patch_pair->second : nullptr;
    
    TerrainPatch patch = TerrainPatch(origo, patch_size, neighbour_patches);
    return &terrain_patches.insert(make_pair(index, patch)).first->second;
}

TerrainPatch* Terrain::get_patch_at(glm::vec2 parameter)
{
    auto index = index_at(parameter);
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
    vec2 parameter = vec2(position.x, position.z);
    TerrainPatch* patch = get_patch_at(parameter);
    double height = patch->get_surface_height_at(parameter);
    return vec3(position.x, height, position.z);
}

vec3 Terrain::get_grass_vector_at(const glm::vec3& position)
{
    vec2 parameter = vec2(position.x, position.z);
    TerrainPatch* patch = get_patch_at(parameter);
    return patch->get_grass_vector_at(parameter);
}
