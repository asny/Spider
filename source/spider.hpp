//
//  spider.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtx/rotate_vector.hpp"
#include "glm.hpp"
#include "Geometry.hpp"
#include <vector>
#include <cmath>

class Spider
{
    class Leg
    {
        glm::vec3 default_foot_pos;
        bool is_moving = false;
        float t = 0.f;
        glm::vec3 origin_foot_pos, destination_foot_pos;
        geogo::VertexID* foot_vertex;
        std::shared_ptr<geogo::Geometry> geometry;
    public:
        Leg(std::shared_ptr<geogo::Geometry> _geometry, const glm::vec3& foot_pos) : geometry(_geometry), default_foot_pos(foot_pos)
        {
            foot_vertex = geometry->create_vertex(foot_pos);
            auto hip_vertex = geometry->create_vertex(0.1f * foot_pos);
            geometry->create_edge(hip_vertex, foot_vertex);
        }
        
        void move(float distance, std::function<double(glm::vec3)>& get_height_at, const glm::mat4& local2world)
        {
            glm::vec3 foot_pos = geometry->position()->at(foot_vertex);
            geometry->position()->at(foot_vertex) = foot_pos - glm::vec3(0.f, 0.f, distance);
            check_should_move(get_height_at, local2world);
        }
        
        void rotate(float distance, std::function<double(glm::vec3)>& get_height_at, const glm::mat4& local2world)
        {
            glm::vec3 foot_pos = geometry->position()->at(foot_vertex);
            geometry->position()->at(foot_vertex) = glm::vec3(glm::rotate(glm::mat4(), -distance, glm::vec3(0.,1.,0.)) * glm::vec4(foot_pos, 1.f));
            check_should_move(get_height_at, local2world);
        }
        
        void update(float time)
        {
            const float move_time = 0.2;
            if(is_moving)
            {
                glm::vec3 foot_pos = geometry->position()->at(foot_vertex);
                t = std::min(t + std::abs(time), move_time);
                auto factor = t / move_time;
                
                foot_pos = factor * destination_foot_pos + (1.f-factor) * origin_foot_pos;
                foot_pos.y += 0.3 * sin(factor * M_PI);
                geometry->position()->at(foot_vertex) = foot_pos;
                
                if(factor >= 1.)
                {
                    is_moving = false;
                    t = 0.f;
                }
            }
        }
        
    private:
        void check_should_move(std::function<double(glm::vec3)>& get_height_at, const glm::mat4& local2world)
        {
            const float radius = 0.5;
            if(!is_moving)
            {
                origin_foot_pos = geometry->position()->at(foot_vertex);
                glm::vec2 foot_xz = glm::vec2(origin_foot_pos.x, origin_foot_pos.z);
                glm::vec2 default_xz = glm::vec2(default_foot_pos.x, default_foot_pos.z);
                if(glm::length(foot_xz - default_xz) > radius)
                {
                    destination_foot_pos = glm::vec3(local2world * glm::vec4(default_foot_pos, 1.f));
                    destination_foot_pos.y = get_height_at(glm::vec3(destination_foot_pos));
                    destination_foot_pos = glm::vec3(glm::inverse(local2world) * glm::vec4(destination_foot_pos, 1.f));
                    is_moving = true;
                }
            }
        }
    };
    
    glm::vec3 position;
    glm::vec3 view_direction;
    std::shared_ptr<geogo::Geometry> legs_geometry;
    std::shared_ptr<glm::mat4> local2world = std::make_shared<glm::mat4>(1.);
    std::function<double(glm::vec3)> get_height_at;
    
    std::vector<Leg> legs;
    
    const float speed = 2.;
    const float angular_speed = 1.;
    const float gravity = -9.82;
    float height;
    glm::vec3 jump_vector;
    bool is_jumping = false;
    
    void update_local2world();
    
public:
    Spider(glm::vec3 _position, glm::vec3 _view_direction, std::function<double(glm::vec3)> _get_height_at) : position(_position), view_direction(_view_direction), get_height_at(_get_height_at)
    {
        height = position.y;
        legs_geometry = std::make_shared<geogo::Geometry>();
        for (auto foot_pos : {glm::vec3(0.75, 0., 2.), glm::vec3(1.,0.,1.), glm::vec3(1.,0.,-0.5), glm::vec3(0.75,0.,-2.),
            glm::vec3(-0.75, 0., 2.), glm::vec3(-1.,0.,1.), glm::vec3(-1.,0.,-0.5), glm::vec3(-0.75,0.,-2.)})
        {
            legs.push_back(Leg(legs_geometry, foot_pos));
        }
        update_local2world();
    }
    
    glm::vec3 get_position();
    glm::vec3 get_view_direction();
    const std::shared_ptr<glm::mat4> get_local2world()
    {
        return local2world;
    }
    
    std::shared_ptr<geogo::Geometry> get_legs()
    {
        return legs_geometry;
    }
    
    void move(float time);
    
    void rotate(float time);
    
    void jump(bool move_forward);
    
    bool update(float time);
};
