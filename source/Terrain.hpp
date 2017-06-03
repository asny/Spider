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
#include "Materials.h"

class Terrain
{
    class TerrainPatch
    {
        struct GrassStraw {
            glm::vec3 position;
            glm::vec3 straw;
        };
        
        glm::vec3 origo;
        std::vector<std::vector<double>> heightmap;
        std::vector<GrassStraw> grass_map;
        
        void set_height(double scale, int r, int c, std::vector<double> neighbour_heights);
        
        void subdivide(int origo_x, int origo_y, int size);
        
    public:
        constexpr const static double SIZE = 8.;
        constexpr const static int VERTICES_PER_UNIT = 4;
        constexpr const static int VERTICES_PER_SIDE = static_cast<int>(SIZE) * VERTICES_PER_UNIT;
        constexpr const static double VERTEX_DISTANCE = 1./static_cast<double>(VERTICES_PER_UNIT);
        const static int NO_GRASS_STRAW_PER_UNIT = 100;
        constexpr const static int NO_GRASS_STRAW = NO_GRASS_STRAW_PER_UNIT * SIZE * SIZE;
        
        TerrainPatch();
        
        void update(const glm::vec3& _origo);
        
        double get_height_at(const glm::vec3& position) const;
        
        glm::vec3 get_origo();
        
        const std::vector<GrassStraw>& get_grass_straws()
        {
            return grass_map;
        }
    };
    const static int PATCH_RADIUS = 1;
    constexpr const static int PATCH_SIDE_LENGTH = 2 * PATCH_RADIUS + 1;
    constexpr const static int VERTICES_PER_SIDE = TerrainPatch::VERTICES_PER_SIDE * PATCH_SIDE_LENGTH;
    constexpr const static double SIZE = TerrainPatch::SIZE * static_cast<double>(PATCH_SIDE_LENGTH);
    
    std::vector<TerrainPatch> patches;
    std::map<std::pair<int,int>, mesh::VertexID*> ground_mapping;
    
    std::shared_ptr<mesh::Mesh> ground_geometry = std::make_shared<mesh::Mesh>();
    std::shared_ptr<mesh::Mesh> grass_geometry = std::make_shared<mesh::Mesh>();
    
    std::shared_ptr<float> time = std::make_shared<float>(0.f);
    std::shared_ptr<glm::vec3> wind_direction = std::make_shared<glm::vec3>(0., 0., 0.);
    std::shared_ptr<glm::vec3> position = std::make_shared<glm::vec3>(0., 0., 0.);
    
    std::pair<int, int> index_at(const glm::vec3& position);
    
    TerrainPatch* patch_at(std::pair<int, int> index);
    
    std::shared_ptr<WaterMaterial> water_material;
    
public:
    
    Terrain(gle::GLScene& scene);
    
    void update(const glm::vec3& position);
    
    double get_height_at(const glm::vec3& position);
    
    void affect_water_at(const glm::vec3& position)
    {
        water_material->affect(position);
    }
};
