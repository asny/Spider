//
//  terrain.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <vector>
#include <map>
#include <math.h>

#include "vec2.hpp"

class TerrainPatch {
    glm::vec2 origo;
    std::vector<std::vector<double>> heightmap;
    
public:
    
    TerrainPatch(glm::vec2 _origo);
    
    std::vector<std::vector<double>> get_heightmap()
    {
        return heightmap;
    }
    
    double get_height_at(glm::vec2 position);
};

class Terrain {
    
    
    std::map<std::pair<int, int>, TerrainPatch> terrain_patches = std::map<std::pair<int, int>, TerrainPatch>();
    
public:
    
    Terrain();
    
    TerrainPatch get_patch_at(glm::vec2 position);
    
};
