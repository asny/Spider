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
#include "Mesh.h"

class TerrainPatch
{
    const static int VERTICES_PER_UNIT = 16;
    constexpr const static double VERTEX_DISTANCE = 1./static_cast<double>(VERTICES_PER_UNIT);
    const static int NO_GRASS_STRAW = 10000;
    
    glm::vec3 origo;
    std::map<std::pair<int,int>, geogo::VertexID*> ground_mapping;
    std::vector<std::vector<double>> heightmap;
    std::shared_ptr<geogo::Mesh> ground_geometry = std::make_shared<geogo::Mesh>();
    std::shared_ptr<geogo::Mesh> water_geometry = std::make_shared<geogo::Mesh>();
    std::shared_ptr<geogo::Mesh> grass_geometry = std::make_shared<geogo::Mesh>();
    std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec2>> ground_uv_coordinates = std::make_shared<geogo::Attribute<geogo::VertexID, glm::vec2>>();
    
    void set_height(double scale, int r, int c, std::vector<double> neighbour_heights);
    
    void subdivide(int origo_x, int origo_y, int size);
    
public:
    constexpr const static double SIZE = 4.;
    
    TerrainPatch();
    
    void update(const glm::vec3& _origo);
    
    double get_height_at(const glm::vec3& position) const;
    
    glm::vec3 get_origo();
    
    std::shared_ptr<geogo::Mesh> get_ground()
    {
        return ground_geometry;
    }
    
    std::shared_ptr<geogo::Mesh> get_water()
    {
        return water_geometry;
    }
    
    std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec2>> get_uv_coordinates()
    {
        return ground_uv_coordinates;
    }
    
    std::shared_ptr<geogo::Mesh> get_grass()
    {
        return grass_geometry;
    }
};

class Terrain
{
    std::vector<TerrainPatch> patches;
    
    std::pair<int, int> index_at(const glm::vec3& position);
    TerrainPatch* patch_at(std::pair<int, int> index);
    
public:
    
    Terrain(const glm::vec3& position);
    
    std::vector<TerrainPatch>& get_patches();
    
    void update(const glm::vec3& position);
    
    double get_height_at(const glm::vec3& position);
};
