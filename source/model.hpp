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
#include "terrain.hpp"

class Model {
    
    Spider spider = Spider(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, -1.f));
    Terrain terrain = Terrain();
    
public:
    Model()
    {
        
    }
    
    // ******** VIEW ********
    
    glm::vec3 get_spider_position()
    {
        return spider.get_position();
    }
    
    glm::vec3 get_spider_view_direction()
    {
        return spider.get_view_direction();
    }
    
    std::vector<glm::vec3> get_terrain()
    {
        glm::vec3 pos = spider.get_position();
        TerrainPatch patch = terrain.get_patch_at(0,0);
        
        std::vector<glm::vec3> terrain;
        for(int y = pos.x - 10; y < pos.y + 10; y++)
        {
            for(int x = pos.x -10; x < pos.y + 10; x++)
            {
                glm::vec2 p = glm::vec2(x,y);
                terrain.push_back(glm::vec3(p, patch.get_height_at(p)));
            }
        }
        
        return terrain;
    }
    
    // ******** CONTROL ********
    
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
