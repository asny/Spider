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
#include "GLScene.h"

class Terrain
{
    class TerrainPatch
    {
        const static int VERTICES_PER_UNIT = 8;
        constexpr const static double VERTEX_DISTANCE = 1./static_cast<double>(VERTICES_PER_UNIT);
        const static int NO_GRASS_STRAW = 10000;
        
        glm::vec3 origo;
        std::map<std::pair<int,int>, mesh::VertexID*> ground_mapping;
        std::vector<std::vector<double>> heightmap;
        std::shared_ptr<mesh::Mesh> ground_geometry = std::make_shared<mesh::Mesh>();
        std::shared_ptr<mesh::Mesh> water_geometry = std::make_shared<mesh::Mesh>();
        std::shared_ptr<mesh::Mesh> grass_geometry = std::make_shared<mesh::Mesh>();
        std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> ground_uv_coordinates = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec2>>();
        std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> water_uv_coordinates = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec2>>();
        
        void set_height(double scale, int r, int c, std::vector<double> neighbour_heights);
        
        void subdivide(int origo_x, int origo_y, int size);
        
    public:
        constexpr const static double SIZE = 8.;
        
        TerrainPatch();
        
        void update(const glm::vec3& _origo);
        
        double get_height_at(const glm::vec3& position) const;
        
        glm::vec3 get_origo();
        
        std::shared_ptr<mesh::Mesh> get_ground()
        {
            return ground_geometry;
        }
        
        std::shared_ptr<mesh::Mesh> get_water()
        {
            return water_geometry;
        }
        
        std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> get_water_uv_coordinates()
        {
            return water_uv_coordinates;
        }
        
        std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> get_uv_coordinates()
        {
            return ground_uv_coordinates;
        }
        
        std::shared_ptr<mesh::Mesh> get_grass()
        {
            return grass_geometry;
        }
    };
    
    std::vector<TerrainPatch> patches;
    
    std::shared_ptr<float> time = std::make_shared<float>(0.f);
    std::shared_ptr<glm::vec3> wind = std::make_shared<glm::vec3>(0., 0., 0.);
    std::shared_ptr<glm::vec3> position = std::make_shared<glm::vec3>(0., 0., 0.);
    
    std::pair<int, int> index_at(const glm::vec3& position);
    
    TerrainPatch* patch_at(std::pair<int, int> index);
    
public:
    
    Terrain(gle::GLScene& scene, const glm::vec3& position);
    
    void update(double time, const glm::vec3& position);
    
    double get_height_at(const glm::vec3& position);
};
