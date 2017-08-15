//
//  Created by Asger Nyman Christiansen on 06/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <future>
#include "GLScene.h"
#include "TerrainPatch.hpp"

class Environment
{
    const static int PATCH_RADIUS = 1;
    constexpr const static int PATCH_SIDE_LENGTH = 2 * PATCH_RADIUS + 1;
    constexpr const static int VERTICES_PER_SIDE = Terrain::VERTICES_PER_SIDE * PATCH_SIDE_LENGTH;
    constexpr const static double SIZE = Terrain::SIZE * static_cast<double>(PATCH_SIDE_LENGTH);
    
    std::pair<int, int> current_center = {0, 0};
    
    std::vector<std::shared_ptr<TerrainPatch>> patches;
    std::map<std::pair<int,int>, mesh::VertexID*> ground_mapping;
    
    gle::GLScene *scene;
    std::shared_ptr<WaterMaterial> water_material;
    std::shared_ptr<mesh::Mesh> ground_geometry = std::make_shared<mesh::Mesh>();
    
    bool is_generating = false;
    std::future<void> fut;
    
    std::pair<int, int> index_at(const glm::vec3& position);
    
    std::shared_ptr<TerrainPatch> patch_at(std::pair<int, int> index);
    
    void update_patches(const std::pair<int, int>& index_at_position);
    
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
