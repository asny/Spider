//
//  model.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "model.hpp"
#include "glm.hpp"

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

void Model::get_terrain(vector<vec3>& positions, vector<vec3>& normals)
{
    vec3 spider_position = spider.get_position();
    
    const double radius = 5.;
    const double step_size = 1./8.;
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
        }
    }
}
