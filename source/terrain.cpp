//
//  terrain.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "glm.hpp"
#include "terrain.hpp"
#include "simplexnoise.h"

using namespace std;
using namespace glm;

double random(double min, double max)
{
    return (max - min) * (double)rand()/(double)RAND_MAX + min;
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

vec3 approximate_normal(const vec3& position, const vec3& neighbour_position)
{
    vec3 tangent1 = normalize(neighbour_position - position);
    vec3 tangent2 = cross(vec3(0., 1., 0.), tangent1);
    return normalize(cross(tangent1, tangent2));
}


vec3 approximate_normal(const vec3& position, const vector<vec3>& neighbour_positions)
{
    vec3 normal = vec3(0.);
    for (vec3 pos : neighbour_positions)
    {
        normal += approximate_normal(position, pos);
    }
    return normalize(normal);
}

TerrainPatch::TerrainPatch()
{
    int map_size = static_cast<int>(SIZE) * VERTICES_PER_UNIT + 1;
    heightmap = vector<vector<double>>(map_size);
    grass = vector<vector<vec3>>(map_size);
    for ( auto r = 0; r < map_size; r++ )
    {
        heightmap[r] = vector<double>(map_size);
        grass[r] = vector<vec3>(map_size);
    }
    for (int r = 0; r < map_size; r++)
    {
        for (int c = 0; c < map_size; c++)
        {
            auto vertex = ground_geometry->create_vertex();
            ground_mapping[pair<int, int>(r,c)] = vertex;
            if(r > 0 && c > 0)
            {
                ground_geometry->create_face(ground_mapping.at(pair<int, int>(r,c-1)), ground_mapping.at(pair<int, int>(r-1,c-1)), vertex);
                ground_geometry->create_face(ground_mapping.at(pair<int, int>(r-1,c)), vertex, ground_mapping.at(pair<int, int>(r-1,c-1)));
            }
            
            auto grass_v1 = grass_geometry->create_vertex();
            auto grass_v2 = grass_geometry->create_vertex();
            grass_mapping[pair<int,int>(r,c)] = grass_geometry->create_edge(grass_v1, grass_v2);
        }
    }
}

void TerrainPatch::update(const vec3& _origo)
{
    origo = _origo;
    int map_size = static_cast<int>(SIZE) * VERTICES_PER_UNIT + 1;
    
    double scale = (map_size - 1) / static_cast<double>(VERTICES_PER_UNIT);
    set_height(scale, 0, 0, {});
    set_height(scale, 0, map_size - 1, {});
    set_height(scale, map_size - 1, 0, {});
    set_height(scale, map_size - 1, map_size - 1, {});
    
    subdivide(0, 0, map_size-1);
    
    double step_size = 1. / static_cast<double>(VERTICES_PER_UNIT);
    for (int r = 0; r < map_size; r++)
    {
        for (int c = 0; c < map_size; c++)
        {
            vec3 pos = vec3(origo.x + r * step_size, heightmap[r][c], origo.z + c * step_size);
            auto ground_vertex = ground_mapping.at(pair<int, int>(r,c));
            ground_geometry->position()->at(ground_vertex) = pos;
            
            auto grass_p1 = pos + vec3(random(-0.5 * step_size, 0.5 * step_size), 0., random(-0.5 * step_size, 0.5 * step_size));
            auto grass_p2 = pos + grass[r][c];
            auto grass_edge = grass_mapping.at(pair<int, int>(r,c));
            grass_geometry->position()->at(grass_edge->v1()) = grass_p1;
            grass_geometry->position()->at(grass_edge->v2()) = grass_p2;
        }
    }
    for (int r = 0; r < map_size; r++)
    {
        for (int c = 0; c < map_size; c++)
        {
            vec3 normal = vec3(0, 1, 0);
            if(r > 0 && c > 0 && r < map_size-1 && c < map_size-1)
            {
                auto pos = ground_geometry->position()->at(ground_mapping.at(pair<int, int>(r,c)));
                auto p1 = ground_geometry->position()->at(ground_mapping.at(pair<int, int>(r,c-1)));
                auto p2 = ground_geometry->position()->at(ground_mapping.at(pair<int, int>(r,c+1)));
                auto p3 = ground_geometry->position()->at(ground_mapping.at(pair<int, int>(r-1,c)));
                auto p4 = ground_geometry->position()->at(ground_mapping.at(pair<int, int>(r+1,c)));
                normal = approximate_normal(pos, {p1, p2, p3, p4});
            }
            ground_normals->at(ground_mapping.at(pair<int, int>(r,c))) = normal;
        }
    }
}

void TerrainPatch::set_height(double scale, int r, int c, std::vector<double> neighbour_heights)
{
    vec3 position = vec3(origo.x + r/static_cast<double>(VERTICES_PER_UNIT), 0., origo.z + c/static_cast<double>(VERTICES_PER_UNIT));
    heightmap[r][c] = average(neighbour_heights) + 0.15 * scale * static_cast<double>(raw_noise_2d(position.x, position.z));
    grass[r][c] = vec3(static_cast<double>(0.25 * raw_noise_3d(position.x + 10., position.z + 10., clock())),
                       static_cast<double>(0.125 * (raw_noise_3d(position.x, position.z, clock()) + 1.) + 0.25),
                       static_cast<double>(0.25 * raw_noise_3d(position.x - 10., position.z - 10., clock())));
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
glm::vec3 TerrainPatch::get_origo()
{
    return origo;
}

Terrain::Terrain(const glm::vec3& position)
{
    update(position);
}

pair<int, int> Terrain::index_at(const vec3& position)
{
    vec2 parameter = vec2(position.x, position.z);
    vec2 index_vector = vec2(floor(parameter.x / TerrainPatch::SIZE), floor(parameter.y / TerrainPatch::SIZE));
    return make_pair(static_cast<int>(index_vector.x), static_cast<int>(index_vector.y));
}

TerrainPatch* Terrain::patch_at(std::pair<int, int> index)
{
    for (auto& patch : patches)
    {
        if(index == index_at(patch.get_origo()))
            return &patch;
    }
    return nullptr;
}

void Terrain::update(const glm::vec3& position)
{
    auto index_at_position = index_at(position);
    
    std::vector<TerrainPatch*> free_patches;
    for (auto& patch : patches)
    {
        auto index = index_at(patch.get_origo());
        if(abs(index_at_position.first - index.first) > 1 || abs(index_at_position.second - index.second) > 1)
        {
            free_patches.push_back(&patch);
        }
    }
    
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            auto index = make_pair(index_at_position.first + i, index_at_position.second + j);
            auto patch = patch_at(index);
            if(!patch)
            {
                TerrainPatch* patch;
                if(free_patches.size() == 0)
                {
                    patches.push_back(TerrainPatch());
                    patch = &patches.back();
                }
                else {
                    patch = free_patches.back();
                    free_patches.pop_back();
                }
                vec3 origo = vec3(TerrainPatch::SIZE * static_cast<double>(index.first), 0., TerrainPatch::SIZE * static_cast<double>(index.second));
                patch->update(origo);
            }
        }
    }
}

std::vector<TerrainPatch>& Terrain::get_patches()
{
    return patches;
}

double Terrain::get_height_at(const glm::vec3& position)
{
    auto index = index_at(position);
    TerrainPatch* patch = patch_at(index);
    return patch->get_surface_height_at(position);
}
