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
    
    std::unique_ptr<Model> model;
    
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
    
    View(int &argc, char** argv);
    
    static View* get_instance()
    {
        return instance;
    }
    
    static void update_terrain()
    {
        for (auto patch_index : View::get_instance()->model->terrain_patches_to_update())
        {
            std::vector<glm::vec3> terrain_positions, terrain_normals, grass_end_points;
            View::get_instance()->model->get_terrain_patch(patch_index.second, terrain_positions, terrain_normals, grass_end_points);
            
            View::get_instance()->terrain_patches[patch_index.first]->update_vertex_attribute("position", terrain_positions);
            View::get_instance()->terrain_patches[patch_index.first]->update_vertex_attribute("normal", terrain_normals);
            View::get_instance()->terrain_patches[patch_index.first]->finalize_vertex_attributes();
            View::get_instance()->grass_patches[patch_index.first]->update_vertex_attribute("end_point", grass_end_points);
            View::get_instance()->grass_patches[patch_index.first]->finalize_vertex_attributes();
        }
    }
    
    void display();
    
    void animate();
    
    void reshape(int width, int height);
    
    void visible(int v);
    
    void mouse(int button, int state, int x, int y);
    
    void keyboard(unsigned char key, int x, int y);
};