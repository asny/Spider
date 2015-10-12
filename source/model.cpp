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

vec3 calculate_normal_at(int row, int col, const vector<vector<vec3>>& positions)
{
    vector<vec3> neighbour_positions;
    if(row > 0)
    {
        neighbour_positions.push_back(positions[row-1][col]);
    }
    if(col > 0)
    {
        neighbour_positions.push_back(positions[row][col-1]);
    }
    if(row + 1 < positions.size())
    {
        neighbour_positions.push_back(positions[row+1][col]);
    }
    if(col + 1 < positions[0].size())
    {
        neighbour_positions.push_back(positions[row][col+1]);
    }
    
    return approximate_normal(positions[row][col], neighbour_positions);
}

void Model::get_terrain(vector<vec3>& positions, vector<vec3>& normals)
{
    vec3 spider_position = spider.get_position();
    vector<vector<vec3>> patch = terrain.get_patch_at(spider_position);
    
    for(int r = 0; r < patch.size() - 1; r++)
    {
        for(int c = 0; c < patch[0].size(); c++)
        {
            auto position = patch[r][c];
            positions.push_back(position);
            
            auto normal = calculate_normal_at(r, c, patch);
            normals.push_back(normal);
            
            if (c == 0)
            {
                positions.push_back(position);
                normals.push_back(normal);
            }
            
            position = patch[r+1][c];
            positions.push_back(position);
            
            normal = calculate_normal_at(r+1, c, patch);
            normals.push_back(normal);
            
            if (c == patch[0].size() - 1)
            {
                positions.push_back(position);
                normals.push_back(normal);
            }
        }
    }
}
