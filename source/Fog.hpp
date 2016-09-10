//
//  Fog.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 10/09/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Geometry.hpp"
#include "mat4x4.hpp"
#include "gtx/rotate_vector.hpp"

class Fog {
    
    std::shared_ptr<geogo::Geometry> geometry = std::make_shared<geogo::Geometry>();;
    std::shared_ptr<glm::mat4> local2world = std::make_shared<glm::mat4>(1.);
    
public:
    Fog()
    {
        geometry->create_vertex(glm::vec3(0., 1.5, -5.));
        geometry->create_vertex(glm::vec3(0., 1.5, -4.));
        geometry->create_vertex(glm::vec3(1., 1., -3.));
    }
    
    const std::shared_ptr<glm::mat4> get_local2world()
    {
        return local2world;
    }
    
    const std::shared_ptr<geogo::Geometry> get_geometry()
    {
        return geometry;
    }
    
    void update(double time)
    {
        
    }
    
    void update_position(const glm::vec3& position)
    {
        *local2world = glm::translate(position);
    }
};

