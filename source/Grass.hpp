//
//  Created by Asger Nyman Christiansen on 03/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Terrain.hpp"

class Grass
{
private:
    std::shared_ptr<mesh::Mesh> geometry = std::make_shared<mesh::Mesh>();
    std::shared_ptr<GrassMaterial> material = std::make_shared<GrassMaterial>();
    
public:
    Grass(gle::GLScene* scene)
    {
        material = std::make_shared<GrassMaterial>();
//        scene->add_leaf(grass_geometry, grass_material);
    }
    
    void update(gle::GLScene* scene, TerrainPatch& patch, float time, const glm::vec3& spider_position);
};
