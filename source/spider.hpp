//
//  spider.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "vec3.hpp"
#include <vector>
#include <cmath>

class Spider {
    glm::vec3 position;
    glm::vec3 view_direction;
    std::vector<glm::vec3> static_feet = {glm::vec3(0.75, 0., 2.), glm::vec3(1.,0.,1.), glm::vec3(1.,0.,-0.5), glm::vec3(0.75,0.,-2.),
        glm::vec3(-0.75, 0., 2.), glm::vec3(-1.,0.,1.), glm::vec3(-1.,0.,-0.5), glm::vec3(-0.75,0.,-2.)};
    float forward_move_time = 0.f;
    float rotate_move_time = 0.f;
    
    const float speed = 2.;
    const float angular_speed = 1.;
    const float gravity = -9.82;
    float height;
    glm::vec3 jump_vector;
    bool is_jumping = false;
    
public:
    Spider(glm::vec3 _position, glm::vec3 _view_direction) : position(_position), view_direction(_view_direction)
    {
        height = position.y;
    }
    
    glm::vec3 get_position();
    glm::vec3 get_view_direction();
    
    std::vector<glm::vec3> get_feet()
    {
        std::vector<glm::vec3> feet = static_feet;
        double offset = 0.;
        for(glm::vec3& f : feet)
        {
            float forward = 0.3 * cos(5. * (forward_move_time + offset));
            float up = 0.3 * std::max(0., cos(5. * (forward_move_time + rotate_move_time + offset)));
            f += glm::vec3(0., up, forward);
            offset += 0.5;
        }
        return feet;
    }
    
    void move(float time);
    
    void rotate(float time);
    
    void jump(bool move_forward);
    
    void update_jump(float time);
};
