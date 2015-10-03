//
//  GUI.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "visualizer.h"

/**
 A GUI application which utilizes OpenGL and GLUT for visualization.
 */
class GUI
{
    std::unique_ptr<Visualizer> visualizer;
    
    glm::vec3 eye_pos = {-10., 5., 10.};
    glm::vec3 light_pos = {0., 5., 5.};
    
    int WIN_SIZE_X = 1280;
    int WIN_SIZE_Y = 720;
    
    bool CONTINUOUS = false;
    
    static GUI* instance;
    
public:
    
    GUI(int &argc, char** argv);
    
    static GUI* get_instance()
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