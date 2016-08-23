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
    const std::vector<glm::vec3> initial_feet = {glm::vec3(0.75, 0., 2.), glm::vec3(1.,0.,1.), glm::vec3(1.,0.,-0.5), glm::vec3(0.75,0.,-2.),
        glm::vec3(-0.75, 0., 2.), glm::vec3(-1.,0.,1.), glm::vec3(-1.,0.,-0.5), glm::vec3(-0.75,0.,-2.)};
    
    std::vector<double> feet_cycle = {0., 0., 0., 0., 0., 0., 0., 0.};
    
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
    
    std::vector<glm::vec3> get_feet();
    
    void move(float time);
    
    void rotate(float time);
    
    void jump(bool move_forward);
    
    bool update_jump(float time);
};
