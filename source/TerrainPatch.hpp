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
    
    std::pair<int, int> index;
    
public:
    TerrainPatch()
    {
        this->add_child(grass);
    }
    
    void animate(float time, const glm::vec3& spider_position, const glm::vec3& wind_direction)
    {
        grass->animate(time, spider_position);
    }
    
    void initialize(const std::pair<int, int>& _index)
    {
        index = _index;
        glm::vec3 origo = glm::vec3(Terrain::SIZE * static_cast<double>(index.first), 0., Terrain::SIZE * static_cast<double>(index.second));
        terrain->initialize(origo);
        grass->initialize(*terrain);
    }
    
    const std::pair<int, int>& get_index()
    {
        return index;
    }
    
    double get_height_at(const glm::vec3& position)
    {
        return terrain->get_height_at(position);
    }
    
};
