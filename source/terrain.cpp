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

TerrainPatch::TerrainPatch(int size_x, int size_y)
{
    heightmap = vector<vector<double>>(size_y);
    for ( vector<double>& row : heightmap )
    {
        row = vector<double>(size_x);
        for ( double& height : row)
        {
            height = (double)(random()/RAND_MAX);
        }
    }
}


Terrain::Terrain()
{
    
}
