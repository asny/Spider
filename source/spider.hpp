//
//  spider.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "vec3.hpp"

class Spider {
    glm::vec3 position;
    glm::vec3 view_direction;
    
    const float speed = 2.;
    const float angular_speed = 1.;
    float jump_speed = 100.f;
    
public:
    Spider(glm::vec3 _position, glm::vec3 _view_direction) : position(_position), view_direction(_view_direction)
    {
        
    }
    
    glm::vec3 get_position();
    glm::vec3 get_view_direction();
    
    void move(float time);
    
    void rotate(float time);
    
    void jump();
    
    void update_jump(float time)
    {
        if(jump_speed < 100.f)
        {
            position.y += time * jump_speed;
            jump_speed -= 5.*time;
            if(jump_speed < -3.f)
            {
                jump_speed = 100.f;
            }
        }
    }
};
