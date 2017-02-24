//
//  model.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "vec3.hpp"
#include "glm.hpp"
#include "gtc/matrix_inverse.hpp"

#include "spider.hpp"
#include "terrain.hpp"

class Model {
    
    std::unique_ptr<Spider> spider;
    std::unique_ptr<Terrain> terrain;
    
    void update_terrain()
    {
        terrain->update(spider->get_position());
    }
    
public:
    Model(gle::GLScene& scene)
    {
        auto initial_position = glm::vec3(0., 0.3, -5.);
        terrain = std::unique_ptr<Terrain>(new Terrain(scene, initial_position));
        
        std::function<double(glm::vec3)> get_height_at = std::bind(&Terrain::get_height_at, terrain.get(), std::placeholders::_1);
        spider = std::unique_ptr<Spider>(new Spider(scene, initial_position, glm::vec3(0., 0., 1.), get_height_at));
    }
    
    // ******** VIEW ********
    
    Spider* get_spider()
    {
        return spider.get();
    }
    
    std::vector<TerrainPatch>& get_terrain_patches()
    {
        return terrain->get_patches();
    }
    
    // ******** CONTROL ********
    
    void move(double time)
    {
        spider->move(time);
        update_terrain();
    }
    
    void rotate(double time)
    {
        spider->rotate(time);
    }
    
    void jump(bool move_forward)
    {
        spider->jump(move_forward);
    }
    
    void update(double time)
    {
        bool is_changed = spider->update(time);
        if(is_changed)
        {
            update_terrain();
        }
    }
};
