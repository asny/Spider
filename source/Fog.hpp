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
    double random(double min, double max)
    {
        return (max - min) * (double)rand()/(double)RAND_MAX + min;
    }
    
    std::shared_ptr<geogo::Geometry> geometry = std::make_shared<geogo::Geometry>();
    std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> trajectory_normals = std::make_shared<geogo::Attribute<geogo::VertexID, glm::vec3>>();
    std::shared_ptr<glm::mat4> local2world = std::make_shared<glm::mat4>(1.);
    
    void create_particle(const glm::vec3& position, const glm::vec3& normal)
    {
        auto vertexId = geometry->create_vertex(position);
        trajectory_normals->at(vertexId) = normal;
    }
    
    void generate_particle()
    {
        float radius = random(2., 10.);
        double theta = random(0., 2. * M_PI);
        double phi = random(0.25 * M_PI, 0.75 * M_PI);
        glm::vec3 direction = glm::vec3(cos(theta) * sin(phi), cos(phi), sin(theta) * sin(phi));
        
        glm::vec3 tangent = cross(direction, glm::vec3(0., 1., 0.));
        glm::vec3 normal = cross(direction, tangent);
        double alpha = random(0., 0.5 * M_PI) - 0.25 * M_PI;
        float sign = rand()%2 == 0 ? -1.f : 1.f;
        normal = sign * normalize(cosf(alpha) * normal + sinf(alpha) * tangent);
        
        create_particle(radius * direction, normal);
    }
    
public:
    Fog(const glm::vec3& position)
    {
        for(int i = 0; i < 2000; i++)
        {
            generate_particle();
        }
        update_position(position);
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

