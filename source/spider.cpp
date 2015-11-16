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
    if(velocity < 100.f)
    {
        position.y += velocity;
        velocity -= 0.001;
        if(velocity < -0.03)
        {
            velocity = 100.f;
        }
    }
    return position;
}

glm::vec3 Spider::get_view_direction()
{
    return view_direction;
}

void Spider::move_forward()
{
    position += stepsize * view_direction;
}

void Spider::move_backwards()
{
    position -= stepsize * view_direction;
}

void Spider::rotate_left()
{
    view_direction = vec3(rotate(mat4(), stepangle, vec3(0.,1.,0.)) * vec4(view_direction, 1.));
}

void Spider::rotate_right()
{
    view_direction = vec3(rotate(mat4(), -stepangle, vec3(0.,1.,0.)) * vec4(view_direction, 1.));
}

void Spider::jump()
{
    if(velocity >= 100.f)
    {
        velocity = 0.03f;
    }
}
