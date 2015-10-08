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

TerrainPatch::TerrainPatch(vec2 _origo, vec2 _size) : origo(_origo), size(_size)
{
    int no_rows = size.y * discretization;
    int no_cols = size.x * discretization;
    
    heightmap = vector<vector<double>>(no_rows);
    for ( auto r = 0; r < no_rows; r++ )
    {
        heightmap[r] = vector<double>(no_cols);
        for ( auto c = 0; c < no_cols; c++ )
        {
            heightmap[r][c] = (double)raw_noise_2d(r, c);
        }
    }
}


double TerrainPatch::get_height_at(glm::vec2 position)
{
    vec2 index = (position - origo) * (float)discretization;
    assert(0 <= index.x <= size.x);
    assert(0 <= index.y <= size.y);
    return heightmap[(int) index.x][(int) index.y];
}


Terrain::Terrain()
{
    
    
}
