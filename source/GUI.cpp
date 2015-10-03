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

GUI::GUI(int &argc, char** argv)
{
    instance = this;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutCreateWindow("Spider");
    glutReshapeWindow(WIN_SIZE_X, WIN_SIZE_Y);
    
    glutDisplayFunc(display_);
    glutKeyboardFunc(keyboard_);
    glutIgnoreKeyRepeat(true);
    glutVisibilityFunc(visible_);
    glutReshapeFunc(reshape_);
    glutIdleFunc(animate_);
    
    visualizer = std::unique_ptr<Visualizer>(new Visualizer());
    
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
    cube.update_data(data);
    visualizer->add_object(cube);
    
    visualizer->set_light_position(light_pos);
    
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
    GLfloat timeValue = glutGet(GLUT_ELAPSED_TIME)*0.001;
    glm::vec3 ep( eye_pos[0] * sinf(timeValue), eye_pos[1] , eye_pos[2] * cosf(timeValue));
    visualizer->set_eye_position(ep);
    glutPostRedisplay();
}

void GUI::keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case '\033':
            exit(0);
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