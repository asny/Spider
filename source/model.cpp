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

vec3 approximate_normal(vec3 pos, vec3 neighbour_pos)
{
    vec3 tangent1 = normalize(neighbour_pos - pos);
    vec3 tangent2 = cross(vec3(0., 1., 0.), tangent1);
    return normalize(cross(tangent1, tangent2));
}

vec3 calculate_normal_at(int row, int col, const vector<vector<vec3>>& positions)
{
    if(row == 0 || col == 0 || row == positions.size() - 1 || col == positions[0].size() - 1)
    {
        return vec3(0., 1., 0.);
    }
    
    vec3 pos_at_origo = positions[row][col];
    
    vec3 normal = approximate_normal(pos_at_origo, positions[row+1][col]);
    normal += approximate_normal(pos_at_origo, positions[row][col+1]);
    normal += approximate_normal(pos_at_origo, positions[row-1][col]);
    normal += approximate_normal(pos_at_origo, positions[row][col-1]);
    
    return normalize(normal);
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
