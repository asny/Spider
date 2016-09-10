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

class Fog
{
    std::shared_ptr<geogo::Geometry> geometry = std::make_shared<geogo::Geometry>();
    std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> trajectory_normals = std::make_shared<geogo::Attribute<geogo::VertexID, glm::vec3>>();
    std::shared_ptr<glm::mat4> local2world = std::make_shared<glm::mat4>(1.);
    
    void create_particle(const glm::vec3& position, const glm::vec3& normal)
    {
        auto vertexId = geometry->create_vertex(position);
        trajectory_normals->at(vertexId) = normal;
    }
    
public:
    Fog()
    {
        create_particle(glm::vec3(0., 1.5, -5.), glm::vec3(0., 0., 1.));
        create_particle(glm::vec3(0., 1.5, -4.), glm::vec3(0., 0., 1.));
        create_particle(glm::vec3(1., 1., -3.), glm::vec3(0., 0., 1.));
    }
    
    const std::shared_ptr<glm::mat4> get_local2world()
    {
        return local2world;
    }
    
    const std::shared_ptr<geogo::Geometry> get_geometry()
    {
        return geometry;
    }
    
    const std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> get_trajectory_normals()
    {
        return trajectory_normals;
    }
    
    void update_position(const glm::vec3& position)
    {
        *local2world = glm::translate(position);
    }
};

