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
    
    Spider spider = Spider(glm::vec3(0., 0.3, -5.), glm::vec3(0., 0., 1.));
    Terrain terrain = Terrain();
    
    std::map<int, std::pair<int, int>> terrainIndexMap = std::map<int, std::pair<int, int>>();
    std::function<void()> on_spider_position_changed;
    std::function<void()> on_spider_view_direction_changed;
    
    glm::vec3 approximate_normal_at(const glm::vec3& position, double filter_size);
    
public:
    Model(std::function<void()> _on_spider_position_changed, std::function<void()> _on_spider_view_direction_changed)
        : on_spider_position_changed(_on_spider_position_changed), on_spider_view_direction_changed(_on_spider_view_direction_changed)
    {
        
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
    
    std::vector<glm::vec3> get_spider_feet_positions(const glm::mat4& modelMatrix)
    {
        glm::mat4 inverseModelMatrix = glm::inverse(modelMatrix);
        
        auto feet = std::vector<glm::vec3>();
        for (auto vec : spider.get_feet())
        {
            glm::vec4 foot_world = modelMatrix * glm::vec4(vec.x, vec.y, vec.z, 1.);
            double terrain_height_world = terrain.get_terrain_position_at(glm::vec3(foot_world.x, foot_world.y, foot_world.z)).y;
            double terrain_height = (inverseModelMatrix * glm::vec4(foot_world.x, terrain_height_world, foot_world.z, 1.)).y;
            feet.push_back(glm::vec3(vec.x, terrain_height + vec.y, vec.z));
        }
        return feet;
    }
    
    std::vector<int> terrain_patches_to_update();
    
    void get_terrain_patch(int patch_index, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& grass_end_points);
    
    // ******** CONTROL ********
    
    void move(double time)
    {
        spider.move(time);
        on_spider_position_changed();
    }
    
    void rotate(double time)
    {
        spider.rotate(time);
        on_spider_view_direction_changed();
    }
    
    void jump(bool move_forward)
    {
        spider.jump(move_forward);
    }
    
    void update_jump(double time)
    {
        spider.update_jump(time);
        on_spider_position_changed();
    }
};
