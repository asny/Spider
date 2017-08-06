//
//  Created by Asger Nyman Christiansen on 06/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Terrain.hpp"

class TerrainPatch
{
    glm::vec3 origo;
    std::vector<std::vector<double>> heightmap;
    
    void set_height(double scale, int r, int c, std::vector<double> neighbour_heights);
    
    void subdivide(int origo_x, int origo_y, int size);
    
public:
    constexpr const static double SIZE = 16.;
    constexpr const static int VERTICES_PER_UNIT = 8;
    constexpr const static int VERTICES_PER_SIDE = static_cast<int>(SIZE) * VERTICES_PER_UNIT;
    constexpr const static double VERTEX_DISTANCE = 1./static_cast<double>(VERTICES_PER_UNIT);
    
    TerrainPatch();
    
    void update(const glm::vec3& _origo);
    
    double get_height_at(const glm::vec3& position) const;
    
    glm::vec3 get_origo();
};
