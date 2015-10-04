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
    
    glm::vec3 get_position()
    {
        return spider.get_position();
    }
    
};
