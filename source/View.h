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
    
    std::unique_ptr<Model> model;
    
    std::shared_ptr<oogl::GLCamera> camera;
    std::unique_ptr<oogl::GLScene> scene;
    
    std::shared_ptr<glm::vec3> light_pos = std::make_shared<glm::vec3>(0., 2000., 2.);
    std::shared_ptr<glm::vec3> wind = std::make_shared<glm::vec3>(0., 0., 0.);
    std::shared_ptr<glm::vec3> spider_pos = std::make_shared<glm::vec3>(0., 0., 0.);
    
    std::shared_ptr<float> time = std::make_shared<float>(0.f);
    
    static View* instance;
    
public:
    
    View(int &argc, char** argv);
    
private:
    
    void create_cube();
    void create_skybox();
    void create_fog();
    void create_spider_body();
    void create_spider_legs();
    void create_grass();
    void create_terrain();
    
    static void update_terrain_and_grass();
    
    static void update_camera();
    
    void update(double elapsedTime);
};