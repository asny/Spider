//
//  controller.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLCamera.hpp"
#include "model.hpp"

class View
{
    enum VIEW_TYPE { FIRST_PERSON, THIRD_PERSON, BIRD };
    
    VIEW_TYPE view_type = FIRST_PERSON;
    
    bool display_grass = true;
    
    std::unique_ptr<Model> model;
    
    std::unique_ptr<oogl::GLCamera> camera;
    
    std::shared_ptr<Geometry> spider_legs_geometry;
    std::shared_ptr<oogl::GLObject> cube, spider_body, spider_legs, skybox;
    std::vector<std::shared_ptr<oogl::GLObject>> terrain_patches = std::vector<std::shared_ptr<oogl::GLObject>>();
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
    
    void display();
    
    void update(double elapsedTime);
};