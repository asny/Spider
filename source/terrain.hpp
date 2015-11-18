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

class TerrainPatch
{
    glm::vec2 origo;
    std::vector<std::vector<double>> heightmap;
    std::vector<std::vector<glm::vec3>> grass;
    std::vector<const TerrainPatch*> neighbour_patches;
    
    void set_height(double scale, int r, int c, std::vector<double> neighbour_heights);
    
    void subdivide(int origo_x, int origo_y, int size);
    
    glm::vec2 index_at(const glm::vec3& position) const;
    
public:
    enum DIRECTION {
        NORTH = 0,
        SOUTH = 1,
        WEST = 2,
        EAST = 3
    };
    
    const static int VERTICES_PER_UNIT = 16;
    
    TerrainPatch(glm::vec2 _origo, double _size, std::vector<const TerrainPatch*> _neighbour_patches);
    
    double get_surface_height_at(const glm::vec3& position) const;
    
    glm::vec3 get_grass_vector_at(const glm::vec3& position) const;
    
    glm::vec2 get_origo()
    {
        return origo;
    }
    
    glm::vec2 get_size()
    {
        return glm::vec2(static_cast<double>(heightmap.size() / VERTICES_PER_UNIT), static_cast<double>(heightmap[0].size() / VERTICES_PER_UNIT));
    }
};

class Terrain {
    
    const double patch_size = 4.;
    
    std::map<std::pair<int, int>, TerrainPatch> terrain_patches = std::map<std::pair<int, int>, TerrainPatch>();
    
    TerrainPatch* create_patch_at(std::pair<int, int> index);
    
public:
    
    Terrain();
    
    std::pair<int, int> index_at(const glm::vec3& position);
    
    TerrainPatch* get_patch_at(std::pair<int, int> index);
    
    glm::vec3 get_terrain_position_at(const glm::vec3& position);
    
    glm::vec3 get_grass_vector_at(const glm::vec3& position);
};
