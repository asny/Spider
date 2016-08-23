//
//  controller.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLScene.hpp"
#include "model.hpp"

class View
{
    enum VIEW_TYPE { FIRST_PERSON, THIRD_PERSON, BIRD };
    
    VIEW_TYPE view_type = FIRST_PERSON;
    
    bool display_grass = true;
    
    std::unique_ptr<Model> model;
    
    std::shared_ptr<oogl::GLCamera> camera;
    std::unique_ptr<oogl::GLScene> scene;
    
    std::shared_ptr<geogo::Geometry> spider_legs_geometry;
    std::shared_ptr<oogl::GLObject> spider_body;
    std::vector<std::shared_ptr<oogl::GLObject>> grass_patches = std::vector<std::shared_ptr<oogl::GLObject>>();
    
    glm::vec3 light_pos = {0., 2000., 2.};
    
    static View* instance;
    
public:
    
    View(int &argc, char** argv);
    
private:
    
    void create_cube();
    void create_skybox();
    void create_spider_body();
    void create_spider_legs();
    void create_grass();
    void create_terrain();
    
    static void update_terrain_and_grass();
    static void update_spider();
    
    static void update_camera();
    
    void update(double elapsedTime);
};