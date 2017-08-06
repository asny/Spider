//
//  Created by Asger Nyman Christiansen on 06/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <future>
#include "GLScene.h"
#include "TerrainPatch.hpp"

class Environment {
    
    
    const static int PATCH_RADIUS = 1;
    constexpr const static int PATCH_SIDE_LENGTH = 2 * PATCH_RADIUS + 1;
    constexpr const static int VERTICES_PER_SIDE = TerrainPatch::VERTICES_PER_SIDE * PATCH_SIDE_LENGTH;
    constexpr const static double SIZE = TerrainPatch::SIZE * static_cast<double>(PATCH_SIDE_LENGTH);
    
    std::vector<TerrainPatch> patches;
    std::map<std::pair<int,int>, mesh::VertexID*> ground_mapping;
    
    std::shared_ptr<mesh::Mesh> ground_geometry = std::make_shared<mesh::Mesh>();
    
    std::shared_ptr<float> time = std::make_shared<float>(0.f);
    std::shared_ptr<glm::vec3> wind_direction = std::make_shared<glm::vec3>(0., 0., 0.);
    std::shared_ptr<glm::vec3> position = std::make_shared<glm::vec3>(0., 0., 0.);
    
    std::pair<int, int> index_at(const glm::vec3& position);
    
    TerrainPatch* patch_at(std::pair<int, int> index);
    
    std::shared_ptr<WaterMaterial> water_material;
    
    bool should_generate_patches(const std::pair<int, int>& index_at_position);
    
    void update_patches(const std::pair<int, int>& index_at_position);
    
    bool is_generating = false;
    std::future<void> fut;
    
    gle::GLScene *scene;
    
public:
    
    Environment(gle::GLScene* scene);
    
    void update(const glm::vec3& position);
    
    bool is_inside(const glm::vec3& position);
    
    double get_height_at(const glm::vec3& position);
    
    void affect_water_at(const glm::vec3& position)
    {
        water_material->affect(position);
    }
    
};
