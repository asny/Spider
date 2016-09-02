//
//  spider.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "vec3.hpp"
#include "glm.hpp"
#include "Geometry.hpp"
#include <vector>
#include <cmath>

class Spider
{
    class Leg
    {
        glm::vec3 initial_foot_pos;
        bool is_moving = false;
        float t = 0.f;
        glm::vec3 destination_vector;
        geogo::VertexID* foot_vertex;
        std::shared_ptr<geogo::Geometry> geometry;
    public:
        Leg(std::shared_ptr<geogo::Geometry> _geometry, const glm::vec3& foot_pos) : geometry(_geometry), initial_foot_pos(foot_pos)
        {
            foot_vertex = geometry->create_vertex(foot_pos);
            auto hip_vertex = geometry->create_vertex(0.1f * foot_pos);
            geometry->create_edge(hip_vertex, foot_vertex);
        }
        
        void move(float spider_move)
        {
            const float radius = 0.5;
            
            glm::vec3 foot_pos = geometry->position()->at(foot_vertex);
            foot_pos.z -= spider_move;
            if(!is_moving)
            {
                auto vec = foot_pos - initial_foot_pos;
                if(glm::length(vec) > radius)
                {
                    destination_vector = vec;
                    is_moving = true;
                    t = 0.f;
                }
            }
            geometry->position()->at(foot_vertex) = foot_pos;
        }
        
        void update(float time)
        {
            const float move_time = 0.5;
            glm::vec3 foot_pos = geometry->position()->at(foot_vertex);
            if(is_moving)
            {
                t = std::min(t + std::abs(time), move_time);
                auto factor = t*t/(move_time * move_time);
                auto origin = initial_foot_pos + destination_vector;
                auto destination = initial_foot_pos - 0.8f * destination_vector;
                foot_pos = factor * destination + (1.f-factor) * origin;
                foot_pos.y = 0.3 * sin(factor * M_PI);
                
                if(factor >= 1.)
                {
                    is_moving = false;
                }
            }
            geometry->position()->at(foot_vertex) = foot_pos;
        }
    };
    
    glm::vec3 position;
    glm::vec3 view_direction;
    std::shared_ptr<geogo::Geometry> legs_geometry;
    
    std::vector<Leg> legs;
    
    const float speed = 2.;
    const float angular_speed = 1.;
    const float gravity = -9.82;
    float height;
    glm::vec3 jump_vector;
    bool is_jumping = false;
    
public:
    Spider(glm::vec3 _position, glm::vec3 _view_direction) : position(_position), view_direction(_view_direction)
    {
        height = position.y;
        legs_geometry = std::make_shared<geogo::Geometry>();
        for (auto foot_pos : {glm::vec3(0.75, 0., 2.), glm::vec3(1.,0.,1.), glm::vec3(1.,0.,-0.5), glm::vec3(0.75,0.,-2.),
            glm::vec3(-0.75, 0., 2.), glm::vec3(-1.,0.,1.), glm::vec3(-1.,0.,-0.5), glm::vec3(-0.75,0.,-2.)})
        {
            legs.push_back(Leg(legs_geometry, foot_pos));
        }
    }
    
    glm::vec3 get_position();
    glm::vec3 get_view_direction();
    
    std::vector<glm::vec3> get_feet();
    
    std::shared_ptr<geogo::Geometry> get_legs()
    {
        return legs_geometry;
    }
    
    void move(float time);
    
    void rotate(float time);
    
    void jump(bool move_forward);
    
    bool update_jump(float time);
};
