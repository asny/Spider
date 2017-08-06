//
//  Created by Asger Nyman Christiansen on 03/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Terrain.hpp"

class Grass : public gle::GLNode
{
private:
    std::shared_ptr<mesh::Mesh> geometry = std::make_shared<mesh::Mesh>();
    std::shared_ptr<GrassMaterial> material = std::make_shared<GrassMaterial>();
    
    const static int NO_GRASS_STRAW_PER_UNIT = 500;
    
public:
    Grass()
    {
        this->add_leaf(geometry, material);
    }
    
    void update(Terrain& patch);
    
    void animate(float time, const glm::vec3& spider_position);
};
