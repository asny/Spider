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
    return view_direction;
}

void Spider::move(float time)
{
    if(!is_jumping)
    {
        position += time * speed * view_direction;
        for (Leg& leg : legs) {
            leg.move(time * speed);
        }
    }
}

void Spider::rotate(float time)
{
    if(!is_jumping)
    {
        view_direction = vec3(glm::rotate(mat4(), time * angular_speed, vec3(0.,1.,0.)) * vec4(view_direction, 1.));
        for (Leg& leg : legs) {
            leg.rotate(time * angular_speed);
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
    for (Leg& leg : legs) {
        leg.update(time);
    }
    if(is_jumping)
    {
        position += time * jump_vector;
        jump_vector.y += time * gravity;
        if(position.y < height)
        {
            position.y = height;
            is_jumping = false;
        }
        return true;
    }
    return false;
}
