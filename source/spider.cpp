//
//  spider.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "spider.hpp"

#include "vec4.hpp"
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"

using namespace glm;

glm::vec3 Spider::get_position()
{
    return position;
}

glm::vec3 Spider::get_view_direction()
{
    return view_direction;
}

void Spider::move(float time)
{
    position += time * speed * view_direction;
}

void Spider::rotate(float time)
{
    if(!is_jumping)
    {
        view_direction = vec3(glm::rotate(mat4(), time * angular_speed, vec3(0.,1.,0.)) * vec4(view_direction, 1.));
    }
}

void Spider::jump()
{
    if(!is_jumping)
    {
        is_jumping = true;
        jump_speed = 3.f;
    }
}

void Spider::update_jump(float time)
{
    if(is_jumping)
    {
        position.y += time * jump_speed;
        jump_speed += time * gravity;
        if(position.y < height)
        {
            position.y = height;
            is_jumping = false;
        }
    }
}
