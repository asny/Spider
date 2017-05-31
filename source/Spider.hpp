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
#include "GLScene.h"
#include "Terrain.hpp"
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
        mesh::VertexID *foot_vertex, *hip_vertex;
        std::shared_ptr<mesh::Mesh> geometry;
        
    public:
        Leg(std::shared_ptr<mesh::Mesh> _geometry, const glm::vec3& foot_pos) : geometry(_geometry), default_foot_pos_local(foot_pos), default_hip_pos_local(0.1f * foot_pos)
        {
            foot_vertex = geometry->create_vertex();
            hip_vertex = geometry->create_vertex();
            geometry->create_edge(hip_vertex, foot_vertex);
        }
        
        void update(const glm::mat4& local2world, Terrain& terrain, float time);
    };
    
    glm::vec3 position;
    glm::vec3 view_direction;
    std::shared_ptr<glm::mat4> local2world = std::make_shared<glm::mat4>(1.);
    
    std::vector<Leg> legs;
    
    const float speed = 2.;
    const float angular_speed = 1.;
    const float gravity = -9.82;
    float height;
    glm::vec3 jump_vector;
    
    // Move states
    bool is_moving_forward = false;
    bool is_moving_backward = false;
    bool is_rotating_right = false;
    bool is_rotating_left = false;
    bool is_jumping = false;
    
public:
    Spider(gle::GLScene& scene, glm::vec3 _position, glm::vec3 _view_direction) : position(_position), view_direction(_view_direction)
    {
        height = position.y;
        create_scene_graph(scene);
    }
    
    glm::vec3 get_position(Terrain& terrain);
    glm::vec3 get_view_direction(Terrain& terrain);
    
    void move_foward(bool value)
    {
        is_moving_forward = value;
    }
    
    void move_backward(bool value)
    {
        is_moving_backward = value;
    }
    
    void rotate_right(bool value)
    {
        is_rotating_right = value;
    }
    
    void rotate_left(bool value)
    {
        is_rotating_left = value;
    }
    
    void jump();
    
    void update(Terrain& terrain, float time);
    
private:
    void create_scene_graph(gle::GLScene& scene);
    
};
