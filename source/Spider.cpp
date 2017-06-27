//
//  spider.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "Spider.hpp"
#include "Materials.h"

using namespace glm;
using namespace gle;
using namespace std;
using namespace mesh;

void Spider::create_scene_graph()
{
    // Create body
    auto spider_transformation = std::make_shared<GLTransformationNode>(local2world);
    scene->add_child(spider_transformation);
    
    auto geometry = shared_ptr<Mesh>(new Mesh());
    auto normals = shared_ptr<Attribute<VertexID, vec3>>(new Attribute<VertexID, vec3>());
    MeshCreator::load_from_obj("resources/spider.obj", *geometry, *normals);
    
    vec3 center;
    for(auto vertex = geometry->vertices_begin(); vertex != geometry->vertices_end(); vertex = vertex->next())
    {
        vec3 pos = geometry->position()->at(vertex);
        center += pos;
    }
    center /= geometry->get_no_vertices();
    
    for(auto vertex = geometry->vertices_begin(); vertex != geometry->vertices_end(); vertex = vertex->next())
    {
        vec3 p = geometry->position()->at(vertex);
        geometry->position()->at(vertex) = p - center + vec3(0,0,0.3);
    }
    auto material = make_shared<GLColorMaterial>(vec3(0.3f, 0.2f, 0.2f), normals);
    spider_transformation->add_leaf(geometry, material);
    
    // Create legs
    auto legs_material = make_shared<GLSpiderLegsMaterial>(glm::vec3(0.3f, 0.2f, 0.2f));
    legs_geometry = std::make_shared<mesh::Mesh>();
    for (auto foot_pos : {glm::vec3(0.75, 0., 2.), glm::vec3(1.,0.,1.), glm::vec3(1.,0.,-0.5), glm::vec3(0.75,0.,-2.),
        glm::vec3(-0.75, 0., 2.), glm::vec3(-1.,0.,1.), glm::vec3(-1.,0.,-0.5), glm::vec3(-0.75,0.,-2.)})
    {
        legs.push_back(Leg(legs_geometry, foot_pos));
    }
    scene->add_leaf(legs_geometry, legs_material);
}

void Spider::Leg::update(const glm::mat4& local2world, Terrain& terrain, float time)
{
    geometry->position()->at(hip_vertex) = glm::vec3(local2world * glm::vec4(default_hip_pos_local, 1.));
    
    if(is_moving)
    {
        const float move_time = 0.2;
        t += std::abs(time);
        
        if(t >= move_time)
        {
            is_moving = false;
            t = 0.f;
            geometry->position()->at(foot_vertex) = destination_foot_pos;
        }
        else
        {
            auto factor = t / move_time;
            glm::vec3 foot_pos = factor * destination_foot_pos + (1.f-factor) * origin_foot_pos;
            foot_pos.y += 0.3 * sin(factor * M_PI);
            
            glm::vec3 old_foot_pos = geometry->position()->at(foot_vertex);
            if(old_foot_pos.y >= 0. && foot_pos.y <= 0.)
            {
                terrain.affect_water_at(foot_pos);
            }
            geometry->position()->at(foot_vertex) = foot_pos;
        }
    }
    else
    {
        const float sqrRadius = 0.75 * 0.75;
        origin_foot_pos = geometry->position()->at(foot_vertex);
        glm::vec3 default_pos = glm::vec3(local2world * glm::vec4(default_foot_pos_local, 1.));
        auto vec = default_pos - origin_foot_pos;
        if(vec.x * vec.x + vec.z * vec.z > sqrRadius)
        {
            destination_foot_pos = glm::vec3(default_pos.x + 0.25 * vec.x, 0.f, default_pos.z + 0.25 * vec.z);
            destination_foot_pos.y = terrain.get_height_at(destination_foot_pos);
            is_moving = true;
        }
    }
}

vec3 Spider::get_position(Terrain& terrain) const
{
    return glm::vec3(position.x, position.y + terrain.get_height_at(position), position.z);
}

vec3 Spider::get_view_direction(Terrain& terrain) const
{
    double height0 = terrain.get_height_at(position);
    double height1 = terrain.get_height_at(position + 0.5f * view_direction);
    double height2 = terrain.get_height_at(position + view_direction);
    double y_view_dir = 0.25 * ((height2 - height0) + (height1 - height0));
    return glm::normalize(glm::vec3(view_direction.x, y_view_dir, view_direction.z));
}

void Spider::jump()
{
    if(!is_jumping)
    {
        is_jumping = true;
        jump_vector = vec3(0.f, 4.f, 0.f);
        if (is_moving_forward) {
            jump_vector += speed * view_direction;
        }
    }
}

void Spider::update(Terrain& terrain, float time)
{
    if(is_jumping)
    {
        position += time * jump_vector;
        jump_vector.y += time * gravity;
        if(position.y < height)
        {
            position.y = height;
            is_jumping = false;
        }
    }
    else {
        if (is_moving_forward)
        {
            position += time * speed * view_direction;
        }
        if (is_moving_backward)
        {
            position -= time * speed * view_direction;
        }
        if (is_rotating_left)
        {
            view_direction = vec3(glm::rotate(mat4(), time * angular_speed, vec3(0.,1.,0.)) * vec4(view_direction, 1.));
        }
        if (is_rotating_right)
        {
            view_direction = vec3(glm::rotate(mat4(), -time * angular_speed, vec3(0.,1.,0.)) * vec4(view_direction, 1.));
        }
    }
    
    // Update terrain
    terrain.update(position);
    
    // Get world position and view direction
    vec3 world_position = get_position(terrain);
    vec3 world_view_direction = get_view_direction(terrain);
    
    // Compute spider model matrix
    mat4 spider_rotation_yaw = orientation(normalize(vec3(world_view_direction.x, 0., world_view_direction.z)), vec3(0., 0., 1.));
    mat4 spider_rotation_pitch = orientation(normalize(vec3(0., world_view_direction.y, 1.)), vec3(0., 0., 1.));
    mat4 spider_translation = translate(world_position);
    *local2world = spider_translation * spider_rotation_yaw * spider_rotation_pitch;
    
    // Update legs
    for (Leg& leg : legs)
    {
        leg.update(*local2world, terrain, time);
    }
    scene->invalidate(legs_geometry);
}
