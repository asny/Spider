//
//  terrain.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <vector>

#include "vec2.hpp"

class TerrainPatch {
    int discretization = 2;
    glm::vec2 origo, size;
    std::vector<std::vector<double>> heightmap;
    
public:
    
    TerrainPatch(glm::vec2 _origo, glm::vec2 _size);
    
    std::vector<std::vector<double>> get_heightmap()
    {
        return heightmap;
    }
    
    double get_height_at(glm::vec2 position);
};

class Terrain {
    
    TerrainPatch terrain_patch = TerrainPatch(glm::vec2(-50., -50.), glm::vec2(100., 100.));
    
public:
    
    Terrain();
    
    TerrainPatch get_patch_at(int x, int y)
    {
        return terrain_patch;
    }
    
};
