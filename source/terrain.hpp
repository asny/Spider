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
#include "vec3.hpp"

class TerrainPatch {
    glm::vec2 origo;
    std::vector<std::vector<double>> heightmap;
    
    void set_height(int r, int c, std::vector<double> neighbour_heights);
    void subdivide(int origo_x, int origo_y, int size);
    
public:
    
    TerrainPatch(glm::vec2 _origo);
    
    glm::vec3 get_position_at(glm::vec2 parameter);
    
    glm::vec2 get_origo()
    {
        return origo;
    }
};

class Terrain {
    
    
    std::map<std::pair<int, int>, TerrainPatch> terrain_patches = std::map<std::pair<int, int>, TerrainPatch>();
    
    
    TerrainPatch get_or_create_patch_at(glm::vec2 parameter);
    
    glm::vec2 pos2par(glm::vec3 pos)
    {
        return glm::vec2(pos.x, pos.z);
    }
    
public:
    
    Terrain();
    
    std::vector<std::vector<glm::vec3>> get_terrain_positions_at(glm::vec3 position);
    
    glm::vec3 get_terrain_position_at(glm::vec3 position);
    
};
