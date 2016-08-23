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
    const static int VERTICES_PER_UNIT = 16;
    
    glm::vec3 origo;
    std::map<std::pair<int,int>, geogo::VertexID*> ground_mapping;
    std::map<std::pair<int,int>, geogo::EdgeID*> grass_mapping;
    std::vector<std::vector<double>> heightmap;
    std::vector<std::vector<glm::vec3>> grass;
    std::shared_ptr<geogo::Geometry> ground_geometry = std::shared_ptr<geogo::Geometry>(new geogo::Geometry());
    std::shared_ptr<geogo::Geometry> grass_geometry = std::shared_ptr<geogo::Geometry>(new geogo::Geometry());
    std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> ground_normals = std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>>(new geogo::Attribute<geogo::VertexID, glm::vec3>());
    
    void set_height(double scale, int r, int c, std::vector<double> neighbour_heights);
    
    void subdivide(int origo_x, int origo_y, int size);
    
    glm::vec2 index_at(const glm::vec3& position) const;
    
public:
    constexpr const static double SIZE = 4.;
    
    TerrainPatch();
    
    void update(const glm::vec3& _origo);
    
    double get_surface_height_at(const glm::vec3& position) const;
    
    glm::vec3 get_origo();
    
    std::shared_ptr<geogo::Geometry> get_ground()
    {
        return ground_geometry;
    }
    
    std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> get_ground_normals()
    {
        return ground_normals;
    }
    
    std::shared_ptr<geogo::Geometry> get_grass()
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
    
    Terrain();
    
    std::vector<TerrainPatch>& get_patches();
    
    void update(const glm::vec3& position);
    
    glm::vec3 get_terrain_position_at(const glm::vec3& position);
};
