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
        float radius = random(4., 6.);
        double theta = random(0., 2. * M_PI);
        double phi = random(0., M_PI);
        glm::vec3 position = radius * glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
        
        glm::vec3 normal;
        if(std::abs(dot(normalize(position), glm::vec3(0., 1., 0.))) < 0.9)
        {
            normal = cross(position, glm::vec3(0., 1., 0.));
        }
        else
        {
            normal = cross(position, glm::vec3(1., 0., 0.));
        }
        double alpha = random(0., 2. * M_PI);
        normal = normalize(cosf(alpha) * normal + sinf(alpha) * cross(position, normal));
        create_particle(position, normal);
    }
    
public:
    Fog(const glm::vec3& position)
    {
        for(int i = 0; i < 1000; i++)
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

