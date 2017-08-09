//
//  Created by Asger Nyman Christiansen on 06/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Terrain.hpp"
#include "Grass.hpp"

class TerrainPatch : public gle::GLNode
{
    std::shared_ptr<Terrain> terrain = std::make_shared<Terrain>();
    std::shared_ptr<Grass> grass = std::make_shared<Grass>();
    
public:
    TerrainPatch()
    {
        this->add_child(grass);
    }
    
    void animate(float time, const glm::vec3& spider_position, const glm::vec3& wind_direction)
    {
        grass->animate(time, spider_position);
    }
    
    void initialize(const glm::vec3& origo)
    {
        terrain->initialize(origo);
        grass->initialize(*terrain);
    }
    
    Terrain& get_terrain()
    {
        return *terrain;
    }
    
};
