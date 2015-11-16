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
    
    const float stepsize = 0.1;
    const float stepangle = 0.08;
    float velocity = 100.f;
    
public:
    Spider(glm::vec3 _position, glm::vec3 _view_direction) : position(_position), view_direction(_view_direction)
    {
        
    }
    
    glm::vec3 get_position();
    glm::vec3 get_view_direction();
    
    void move_forward();
    void move_backwards();
    void rotate_left();
    void rotate_right();
    void jump();
};
