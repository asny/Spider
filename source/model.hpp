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
#include "fog.hpp"

class Model {
    
    std::unique_ptr<Spider> spider;
    std::unique_ptr<Terrain> terrain;
    std::unique_ptr<Fog> fog;
    
    std::map<int, std::pair<int, int>> terrainIndexMap = std::map<int, std::pair<int, int>>();
    std::function<void()> on_spider_position_changed;
    std::function<void()> on_spider_view_direction_changed;
    
    void update_terrain()
    {
        terrain->update(spider->get_position());
    }
    
public:
    Model(std::function<void()> _on_spider_position_changed, std::function<void()> _on_spider_view_direction_changed)
        : on_spider_position_changed(_on_spider_position_changed), on_spider_view_direction_changed(_on_spider_view_direction_changed)
    {
        auto initial_position = glm::vec3(0., 0.3, -5.);
        terrain = std::unique_ptr<Terrain>(new Terrain(initial_position));
        
        using namespace std::placeholders;
        std::function<double(glm::vec3)> get_height_at = std::bind(&Terrain::get_height_at, terrain.get(), _1);
        spider = std::unique_ptr<Spider>(new Spider(initial_position, glm::vec3(0., 0., 1.), get_height_at));
        
        fog = std::unique_ptr<Fog>(new Fog());
    }
    
    // ******** VIEW ********
    
    Spider* get_spider()
    {
        return spider.get();
    }
    
    Fog* get_fog()
    {
        return fog.get();
    }
    
    std::vector<TerrainPatch>& get_terrain_patches()
    {
        return terrain->get_patches();
    }
    
    // ******** CONTROL ********
    
    void move(double time)
    {
        spider->move(time);
        on_spider_position_changed();
        update_terrain();
        fog->update_position(spider->get_position());
    }
    
    void rotate(double time)
    {
        spider->rotate(time);
        on_spider_view_direction_changed();
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
            on_spider_position_changed();
            update_terrain();
            fog->update_position(spider->get_position());
        }
        
        fog->update(time);
    }
};
