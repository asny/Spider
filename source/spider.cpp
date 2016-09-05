//
//  spider.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "spider.hpp"

using namespace glm;

glm::vec3 Spider::get_position()
{
    return glm::vec3(position.x, position.y + get_height_at(position), position.z);
}

glm::vec3 Spider::get_view_direction()
{
    double height0 = get_height_at(position);
    double height1 = get_height_at(position + 0.5f * view_direction);
    double height2 = get_height_at(position + view_direction);
    double y_view_dir = 0.25 * ((height2 - height0) + (height1 - height0));
    return glm::normalize(glm::vec3(view_direction.x, y_view_dir, view_direction.z));
}

void Spider::update_local2world()
{
    vec3 spider_position = get_position();
    vec3 spider_view_direction = get_view_direction();
    
    // Compute spider model matrix
    mat4 spider_rotation_yaw = orientation(normalize(vec3(spider_view_direction.x, 0., spider_view_direction.z)), vec3(0., 0., 1.));
    mat4 spider_rotation_pitch = orientation(normalize(vec3(0., spider_view_direction.y, 1.)), vec3(0., 0., 1.));
    mat4 spider_translation = translate(mat4(), spider_position);
    *local2world = spider_translation * spider_rotation_yaw * spider_rotation_pitch;
}

void Spider::move(float time)
{
    if(!is_jumping)
    {
        position += time * speed * view_direction;
        update_local2world();
        for (Leg& leg : legs) {
            leg.move(time * speed, get_height_at, *local2world);
        }
    }
}

void Spider::rotate(float time)
{
    if(!is_jumping)
    {
        view_direction = vec3(glm::rotate(mat4(), time * angular_speed, vec3(0.,1.,0.)) * vec4(view_direction, 1.));
        update_local2world();
        for (Leg& leg : legs) {
            leg.rotate(time * angular_speed, get_height_at, *local2world);
        }
    }
}

void Spider::jump(bool move_forward)
{
    if(!is_jumping)
    {
        is_jumping = true;
        jump_vector = vec3(0.f, 4.f, 0.f);
        if (move_forward) {
            jump_vector += speed * view_direction;
        }
    }
}

bool Spider::update(float time)
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
        update_local2world();
        return true;
    }
    for (Leg& leg : legs) {
        leg.update(time);
    }
    return false;
}
