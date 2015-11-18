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
    
    std::shared_ptr<Model> model;
    
    std::unique_ptr<oogl::GLCamera> camera;
    
    std::shared_ptr<oogl::GLObject> cube, spider, skybox;
    
    std::vector<std::shared_ptr<oogl::GLObject>> terrain_patches = std::vector<std::shared_ptr<oogl::GLObject>>();
    std::vector<std::shared_ptr<oogl::GLObject>> grass_patches = std::vector<std::shared_ptr<oogl::GLObject>>();
    
    glm::vec3 light_pos = {0., 2000., 2.};
    
    int WIN_SIZE_X = 1280;
    int WIN_SIZE_Y = 720;
    
    static View* instance;
    
    void create_cube(std::shared_ptr<oogl::GLShader> shader);
    void create_skybox(std::shared_ptr<oogl::GLShader> shader);
    void create_spider(std::shared_ptr<oogl::GLShader> shader);
    void create_grass(std::shared_ptr<oogl::GLShader> shader);
    void create_terrain(std::shared_ptr<oogl::GLShader> shader);
    
public:
    
    View(std::shared_ptr<Model> _model, int &argc, char** argv);
    
    static View* get_instance()
    {
        return instance;
    }
    
    void display();
    
    void animate();
    
    void reshape(int width, int height);
    
    void visible(int v);
    
    void mouse(int button, int state, int x, int y);
    
    void keyboard(unsigned char key, int x, int y);
};