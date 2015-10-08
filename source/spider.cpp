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