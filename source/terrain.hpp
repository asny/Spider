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
#include "Geometry.hpp"

class TerrainPatch
{
    glm::vec3 origo;
    std::vector<std::vector<double>> heightmap;
    std::vector<std::vector<glm::vec3>> grass;
    std::shared_ptr<geogo::Geometry> ground_geometry = std::shared_ptr<geogo::Geometry>(new geogo::Geometry());
    std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> ground_normals = std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>>(new geogo::Attribute<geogo::VertexID, glm::vec3>());
    
    void set_height(double scale, int r, int c, std::vector<double> neighbour_heights);
    
    void subdivide(int origo_x, int origo_y, int size);
    
    glm::vec2 index_at(const glm::vec3& position) const;
    
public:
    
    const static int VERTICES_PER_UNIT = 16;
    
    TerrainPatch(const glm::vec3& _origo, double _size);
    
    double get_surface_height_at(const glm::vec3& position) const;
    
    glm::vec3 get_grass_vector_at(const glm::vec3& position) const;
    
    glm::vec3 get_origo();
    
    glm::vec3 get_size();
    
    std::shared_ptr<geogo::Geometry> get_ground()
    {
        return ground_geometry;
    }
    
    std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> get_ground_normals()
    {
        return ground_normals;
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
