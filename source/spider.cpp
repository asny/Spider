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
    if(!is_jumping)
    {
        position += time * speed * view_direction;
        
        static double t = 0.;
        t = fmod(t + 5. * time, 2. * M_PI);
        
        double offset = 0.;
        for(glm::vec3& f : feet)
        {
            double distance_foot_to_ground = 0.3 * cos(t + offset);
            
            if(distance_foot_to_ground > 0.)
            {
                f.y = distance_foot_to_ground;
                f.z += time * speed;
            }
            else {
                f.z -= time * speed;
            }
            offset += 0.5 * M_PI;
        }
    }
}

void Spider::rotate(float time)
{
    if(!is_jumping)
    {
        view_direction = vec3(glm::rotate(mat4(), time * angular_speed, vec3(0.,1.,0.)) * vec4(view_direction, 1.));
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

void Spider::update_jump(float time)
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
}
