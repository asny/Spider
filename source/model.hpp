//
//  model.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "vec3.hpp"

#include "spider.hpp"

class Model {
    
    Spider spider = Spider(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, -1.f));
    
public:
    Model()
    {
        
    }
    
    glm::vec3 get_spider_position()
    {
        return spider.get_position();
    }
    
    glm::vec3 get_view_direction()
    {
        return spider.get_view_direction();
    }
    
    void move_forward()
    {
        spider.move_forward();
    }
    
    void move_backwards()
    {
        spider.move_backwards();
    }
    
    void rotate_left()
    {
        spider.rotate_left();
    }
    
    void rotate_right()
    {
        spider.rotate_right();
    }
};
