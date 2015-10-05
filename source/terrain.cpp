//
//  terrain.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "terrain.hpp"

#include <random>

using namespace std;
using namespace glm;

TerrainPatch::TerrainPatch(vec2 _origo, vec2 _size) : origo(_origo), size(_size)
{
    heightmap = vector<vector<double>>(size.y);
    for ( vector<double>& row : heightmap )
    {
        row = vector<double>(size.x);
        for ( double& height : row)
        {
            height = (double)rand()/(double)RAND_MAX - 3.;
        }
    }
}


double TerrainPatch::get_height_at(glm::vec2 position)
{
    vec2 index = position - origo;
    assert(0 <= index.x <= size.x);
    assert(0 <= index.y <= size.y);
    return heightmap[(int) index.x][(int) index.y];
}


Terrain::Terrain()
{
    
    
}
