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

vec3 Model::approximate_normal_at(const vec3& position, double filter_size)
{
    vector<vec3> neighbour_positions;
    vec3 p = position + vec3(filter_size, 0., 0.);
    neighbour_positions.push_back(terrain.get_terrain_position_at(p));
    
    p = position - vec3(filter_size, 0., 0.);
    neighbour_positions.push_back(terrain.get_terrain_position_at(p));
    
    p = position + vec3(0., 0., filter_size);
    neighbour_positions.push_back(terrain.get_terrain_position_at(p));
    
    p = position - vec3(0., 0., filter_size);
    neighbour_positions.push_back(terrain.get_terrain_position_at(p));
    
    return approximate_normal(position, neighbour_positions);
}

map<int, pair<int, int>> Model::terrain_patches_to_update()
{
    vec3 spider_pos = spider.get_position();
    pair<int, int> center = terrain.index_at(spider_pos);
    
    if (terrainIndexMap.size() == 0) {
        for (int i = 0; i < 9; i++) {
            terrainIndexMap[i] = {center.first - 1 + i % 3, center.second - 1 + std::floor(i/3)};
        }
        return terrainIndexMap;
    }
    
    map<int, pair<int, int>> to_update = map<int, pair<int, int>>();
    for (int i = 0; i < 9; i++)
    {
        auto current = terrainIndexMap[i];
        if(center.first - current.first == 2 && center.second - current.second == 2)
        {
            to_update[i] = {center.first + 1, center.second + 1};
        }
        else if(center.first - current.first == -2 && center.second - current.second == -2)
        {
            to_update[i] = {center.first - 1, center.second - 1};
        }
        else if(center.first - current.first == 2 && center.second - current.second == -2)
        {
            to_update[i] = {center.first + 1, center.second - 1};
        }
        else if(center.first - current.first == -2 && center.second - current.second == 2)
        {
            to_update[i] = {center.first - 1, center.second + 1};
        }
        else if(center.first - current.first == 2)
        {
            to_update[i] = {center.first + 1, current.second};
        }
        else if(center.first - current.first == -2)
        {
            to_update[i] = {center.first - 1, current.second};
        }
        else if(center.second - current.second == 2)
        {
            to_update[i] = {current.first, center.second + 1};
        }
        else if(center.second - current.second == -2)
        {
            to_update[i] = {current.first, center.second - 1};
        }
    }
    
    return to_update;
}

double random(double min, double max)
{
    return (max - min) * (double)rand()/(double)RAND_MAX + min;
}

void Model::get_terrain_patch(pair<int, int> patch_index, vector<vec3>& positions, vector<vec3>& normals, vector<vec3>& grass_end_points)
{
    vec3 spider_position = spider.get_position();
    TerrainPatch* patch = terrain.get_patch_at(patch_index);
    
    const double radius = 10.;
    const double step_size = 1./Terrain::VERTICES_PER_UNIT;
    for(double x = -radius; x <= radius; x += step_size)
    {
        for(double z = -radius; z <= radius; z += step_size)
        {
            vec3 p = spider_position + vec3(x, 0., z);
            
            vec3 position = terrain.get_terrain_position_at(p);
            positions.push_back(position);
            
            vec3 normal = approximate_normal_at(position, step_size);
            normals.push_back(normal);
            
            if (z == -radius)
            {
                positions.push_back(position);
                normals.push_back(normal);
            }
            
            p.x += step_size;
            
            position = terrain.get_terrain_position_at(p);
            positions.push_back(position);
            
            normal = approximate_normal_at(position, step_size);
            normals.push_back(normal);
            
            if (z == radius)
            {
                positions.push_back(position);
                normals.push_back(normal);
            }
            
            vec3 grass_vector = terrain.get_grass_vector_at(p);
            grass_end_points.push_back(position + vec3(random(-0.5 * step_size, 0.5 * step_size), 0., random(-0.5 * step_size, 0.5 * step_size)));
            grass_end_points.push_back(position + grass_vector);
        }
    }
}
