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

#include "spider.hpp"
#include "terrain.hpp"

class Model {
    
    Spider spider = Spider(glm::vec3(0., 0.05, -5.), glm::vec3(0., 0., 1.));
    Terrain terrain = Terrain();
    
    std::map<int, std::pair<int, int>> terrainIndexMap = std::map<int, std::pair<int, int>>();
    
    glm::vec3 approximate_normal_at(const glm::vec3& position, double filter_size);
    
    std::function<void(int, const std::vector<glm::vec3>&, const std::vector<glm::vec3>&, const std::vector<glm::vec3>&)> on_update_terrain;
    std::map<int, std::pair<int, int>> terrain_patches_to_update();
    void get_terrain_patch(std::pair<int, int> patch_index, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& grass_end_points);
    void update_terrain()
    {
        for (auto patch_index : terrain_patches_to_update())
        {
            std::vector<glm::vec3> terrain_positions, terrain_normals, grass_end_points;
            get_terrain_patch(patch_index.second, terrain_positions, terrain_normals, grass_end_points);
            on_update_terrain(patch_index.first, terrain_positions, terrain_normals, grass_end_points);
        }
    }
    
    
public:
    Model(std::function<void(int, const std::vector<glm::vec3>&, const std::vector<glm::vec3>&, const std::vector<glm::vec3>&)> _on_update_terrain) : on_update_terrain(_on_update_terrain)
    {
        update_terrain();
    }
    
    // ******** VIEW ********
    
    glm::vec3 get_spider_position()
    {
        glm::vec3 pos = spider.get_position();
        double height = terrain.get_terrain_position_at(pos).y;
        return glm::vec3(pos.x, pos.y + height, pos.z);
    }
    
    glm::vec3 get_spider_view_direction()
    {
        glm::vec3 view_dir = spider.get_view_direction();
        glm::vec3 pos = spider.get_position();
        double height0 = terrain.get_terrain_position_at(pos).y;
        double height1 = terrain.get_terrain_position_at(pos + 0.5f * view_dir).y;
        double height2 = terrain.get_terrain_position_at(pos + view_dir).y;
        double y_view_dir = 0.25 * ((height2 - height0) + (height1 - height0));
        return glm::normalize(glm::vec3(view_dir.x, y_view_dir, view_dir.z));
    }
    
    // ******** CONTROL ********
    
    void move_forward()
    {
        spider.move_forward();
        update_terrain();
    }
    
    void move_backwards()
    {
        spider.move_backwards();
        update_terrain();
    }
    
    void rotate_left()
    {
        spider.rotate_left();
    }
    
    void rotate_right()
    {
        spider.rotate_right();
    }
    
    void jump()
    {
        spider.jump();
    }
};
