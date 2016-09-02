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
    
    std::map<int, std::pair<int, int>> terrainIndexMap = std::map<int, std::pair<int, int>>();
    std::function<void()> on_spider_position_changed;
    std::function<void()> on_spider_view_direction_changed;
    
public:
    Model(std::function<void()> _on_spider_position_changed, std::function<void()> _on_spider_view_direction_changed)
        : on_spider_position_changed(_on_spider_position_changed), on_spider_view_direction_changed(_on_spider_view_direction_changed)
    {
        auto initial_position = glm::vec3(0., 0.3, -5.);
        terrain = std::unique_ptr<Terrain>(new Terrain(initial_position));
        spider = std::unique_ptr<Spider>(new Spider(initial_position, glm::vec3(0., 0., 1.)));
        update_terrain();
    }
    
    // ******** VIEW ********
    
    glm::vec3 get_spider_position()
    {
        glm::vec3 pos = spider->get_position();
        double height = terrain->get_height_at(pos);
        return glm::vec3(pos.x, pos.y + height, pos.z);
    }
    
    glm::vec3 get_spider_view_direction()
    {
        glm::vec3 view_dir = spider->get_view_direction();
        glm::vec3 pos = spider->get_position();
        double height0 = terrain->get_height_at(pos);
        double height1 = terrain->get_height_at(pos + 0.5f * view_dir);
        double height2 = terrain->get_height_at(pos + view_dir);
        double y_view_dir = 0.25 * ((height2 - height0) + (height1 - height0));
        return glm::normalize(glm::vec3(view_dir.x, y_view_dir, view_dir.z));
    }
    
    Spider* get_spider()
    {
        return spider.get();
    }
    
    std::vector<TerrainPatch>& get_terrain_patches()
    {
        return terrain->get_patches();
    }
    
    void update_terrain()
    {
        terrain->update(spider->get_position());
    }
    
    // ******** CONTROL ********
    
    void move(double time)
    {
        spider->move(time);
        on_spider_position_changed();
        update_terrain();
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
        }
    }
};
