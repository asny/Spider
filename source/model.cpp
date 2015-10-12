//
//  model.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "model.hpp"

using namespace std;
using namespace glm;

vec3 calculate_normal_at(int row, int col, const vector<vector<vec3>>& positions)
{
    vec3 pos_at_origo = positions[row][col];
    for (int r = -1; r <= 1; r++) {
        for (int c = -1; c <= 1; c++) {
            if(0 <= r && r <= positions.size() && 0 <= c && c <= positions[0].size())
            {
                
            }
        }
    }
    return vec3(0., 1., 0.);
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
