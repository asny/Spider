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
#include "Mesh.h"
#include <vector>
#include <cmath>

class Spider
{
    class Leg
    {
        glm::vec3 default_foot_pos_local, default_hip_pos_local;
        bool is_moving = false;
        float t = 0.f;
        glm::vec3 origin_foot_pos, destination_foot_pos;
        geogo::VertexID *foot_vertex, *hip_vertex;
        std::shared_ptr<geogo::Mesh> geometry;
        
    public:
        Leg(std::shared_ptr<geogo::Mesh> _geometry, const glm::vec3& foot_pos) : geometry(_geometry), default_foot_pos_local(foot_pos), default_hip_pos_local(0.1f * foot_pos)
        {
            foot_vertex = geometry->create_vertex();
            hip_vertex = geometry->create_vertex();
            geometry->create_edge(hip_vertex, foot_vertex);
        }
        
        void update(const glm::mat4& local2world, std::function<double(glm::vec3)> get_height_at);
        
        void update(float time);
    };
    
    glm::vec3 position;
    glm::vec3 view_direction;
    std::shared_ptr<geogo::Mesh> legs_geometry;
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
        legs_geometry = std::make_shared<geogo::Mesh>();
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
    
    std::shared_ptr<geogo::Mesh> get_legs()
    {
        return legs_geometry;
    }
    
    void move(float time);
    
    void rotate(float time);
    
    void jump(bool move_forward);
    
    bool update(float time);
};
