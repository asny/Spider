//
//  GUI.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GUI.h"

#include <GLUT/glut.h>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <random>
#include <algorithm>
#include <atomic>

void display_(){
    GUI::get_instance()->display();
}

void keyboard_(unsigned char key, int x, int y){
    GUI::get_instance()->keyboard(key, x, y);
}

void reshape_(int width, int height){
    GUI::get_instance()->reshape(width, height);
}

void visible_(int v){
    GUI::get_instance()->visible(v);
}

void animate_(){
    GUI::get_instance()->animate();
}

GUI* GUI::instance = NULL;

static const glm::vec3 cube_data[] = {
    glm::vec3(-1.0f,-1.0f,-1.0f), // triangle 1 : begin
    glm::vec3(-1.0f,-1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f), // triangle 1 : end
    glm::vec3(1.0f, 1.0f,-1.0f), // triangle 2 : begin
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f,-1.0f), // triangle 2 : end
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(1.0f,-1.0f,-1.0f),
    glm::vec3(1.0f, 1.0f,-1.0f),
    glm::vec3(1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f,-1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3(-1.0f,-1.0f, 1.0f),
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f,-1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f,-1.0f),
    glm::vec3(1.0f, 1.0f,-1.0f),
    glm::vec3(1.0f,-1.0f,-1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f,-1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f)
};



static const glm::vec3 triangles_data[] = {
        glm::vec3(0.), glm::vec3(0.,0.,1.), glm::vec3(1.,0.,0.), glm::vec3(0.,0.,1.), glm::vec3(0.,1.,0.), glm::vec3(0.,0.,1.),
        glm::vec3(0.), glm::vec3(1.,0.,0.), glm::vec3(-1.,0.,-1.), glm::vec3(1.,0.,0.), glm::vec3(0.,1.,0.), glm::vec3(1.,0.,0.),
        glm::vec3(0.), glm::vec3(0.,0.,1.), glm::vec3(0.,-1.,0.), glm::vec3(0.,0.,1.), glm::vec3(1.,0.,0.), glm::vec3(0.,0.,1.)
};

GUI::GUI(int &argc, char** argv)
{
    instance = this;
    
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutCreateWindow("Spider");
    glutReshapeWindow(WIN_SIZE_X, WIN_SIZE_Y);
    
    glutDisplayFunc(display_);
    glutKeyboardFunc(keyboard_);
    glutVisibilityFunc(visible_);
    glutReshapeFunc(reshape_);
    glutIdleFunc(animate_);
    
    // Initialize the visualization
    visualizer = std::unique_ptr<Visualizer>(new Visualizer());
    
    create_shaders_and_objects();
    
    visualizer->set_light_position(light_pos);
    visualizer->set_eye_position(eye_pos);
    
    glutMainLoop();
}

void GUI::display()
{
    if (glutGet(GLUT_WINDOW_WIDTH) != WIN_SIZE_X || glutGet(GLUT_WINDOW_HEIGHT) != WIN_SIZE_Y) {
        return;
    }
    visualizer->draw();
    glutSwapBuffers();
}

void GUI::reshape(int width, int height)
{
    WIN_SIZE_X = width;
    WIN_SIZE_Y = height;
    visualizer->reshape(width, height);
    glutPostRedisplay();
}

void GUI::animate()
{
    glutPostRedisplay();
}

void GUI::keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case '\033':
            exit(0);
            break;
        case 'w':
            visualizer->move_forward();
            break;
        case 'a':
            visualizer->rotate_left();
            break;
        case 'd':
            visualizer->rotate_right();
            break;
        case 's':
            visualizer->move_backwards();
            break;
        case ' ':
            if(!CONTINUOUS)
            {
                std::cout << "MOTION STARTED" << std::endl;
            }
            else {
                std::cout << "MOTION PAUSED" << std::endl;
            }
            CONTINUOUS = !CONTINUOUS;
            break;
    }
}

void GUI::visible(int v)
{
    if(v==GLUT_VISIBLE)
        glutIdleFunc(animate_);
    else
        glutIdleFunc(0);
}

void GUI::create_shaders_and_objects()
{
    // Create shaders
    auto shader = GLShader("shaders/gouraud.vert",  "shaders/gouraud.frag");
    visualizer->add_shader(shader);
    
    // Create objects
    auto material = GLMaterial {{0.15f,0.4f,0.5f, 1.f}, {0.2f, 0.3f, 0.4f, 1.f}, {0.2f, 0.3f, 0.4f, 1.f}};
    auto cube = GLObject(shader, material);
    std::vector<glm::vec3> data;
    for (auto pos : cube_data)
    {
        data.push_back(pos);
        data.push_back(glm::vec3(0.,1.,0.));
    }
    cube.set_data(data);
    visualizer->add_object(cube);
    
    material = GLMaterial {{0.55f,0.4f,0.5f, 1.f}, {0.2f, 0.3f, 0.4f, 1.f}, {0.2f, 0.3f, 0.4f, 1.f}};
    auto triangles = GLObject(shader, material);
    data.clear();
    for (auto pos : triangles_data)
    {
        data.push_back(2.f*pos);
        data.push_back(glm::vec3(0.,1.,0.));
    }
    triangles.set_data(data);
    visualizer->add_object(triangles);
}
