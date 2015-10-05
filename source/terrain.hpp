//
//  terrain.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <vector>

class TerrainPatch {
    std::vector<std::vector<double>> heightmap;
    
public:
    
    TerrainPatch(int size_x, int size_y);
    
};

class Terrain {
    
    std::vector<std::vector<TerrainPatch>> terrain_patches;
    
    
public:
    
    Terrain();
    
};
