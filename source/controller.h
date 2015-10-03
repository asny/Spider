//
//  controller.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "view.h"

/**
 A Controller application which utilizes OpenGL and GLUT for visualization.
 */
class Controller
{
    std::unique_ptr<View> visualizer;
    
    glm::vec3 eye_pos = {0., 0., 5.};
    glm::vec3 light_pos = {0., 2., 2.};
    
    int WIN_SIZE_X = 1280;
    int WIN_SIZE_Y = 720;
    
    bool CONTINUOUS = false;
    
    static Controller* instance;
    
public:
    
    Controller(int &argc, char** argv);
    
    static Controller* get_instance()
    {
        return instance;
    }
    
    void display();
    
    void animate();
    
    void reshape(int width, int height);
    
    void visible(int v);
    
    void mouse(int button, int state, int x, int y);
    
    void keyboard(unsigned char key, int x, int y);
    
    void create_shaders_and_objects();
};