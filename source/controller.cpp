//
//  controller.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "Controller.h"

#include <GLUT/glut.h>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <random>
#include <algorithm>
#include <atomic>

void display_(){
    Controller::get_instance()->display();
}

void keyboard_(unsigned char key, int x, int y){
    Controller::get_instance()->keyboard(key, x, y);
}

void reshape_(int width, int height){
    Controller::get_instance()->reshape(width, height);
}

void visible_(int v){
    Controller::get_instance()->visible(v);
}

void animate_(){
    Controller::get_instance()->animate();
}

Controller* Controller::instance = NULL;

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

Controller::Controller(std::shared_ptr<Model> _model, int &argc, char** argv)
{
    instance = this;
    model = _model;
    
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
    visualizer = std::unique_ptr<GLWrapper>(new GLWrapper());
    
    create_shaders_and_objects();
    
    visualizer->set_light_position(light_pos);
    visualizer->set_view(model->get_spider_position(), model->get_view_direction());
    
    glutMainLoop();
}

void Controller::display()
{
    if (glutGet(GLUT_WINDOW_WIDTH) != WIN_SIZE_X || glutGet(GLUT_WINDOW_HEIGHT) != WIN_SIZE_Y) {
        return;
    }
    visualizer->draw();
    glutSwapBuffers();
}

void Controller::reshape(int width, int height)
{
    WIN_SIZE_X = width;
    WIN_SIZE_Y = height;
    visualizer->reshape(width, height);
    glutPostRedisplay();
}

void Controller::animate()
{
    GLfloat timeValue = glutGet(GLUT_ELAPSED_TIME)*0.001;
    glm::vec3 animation(sinf(timeValue), cosf(timeValue) , 0.f);
    visualizer->set_view(model->get_spider_position() + animation, model->get_view_direction());
    glutPostRedisplay();
}

void Controller::keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case '\033':
            exit(0);
            break;
        case 'w':
            model->move_forward();
            break;
        case 'a':
            model->rotate_left();
            break;
        case 'd':
            model->rotate_right();
            break;
        case 's':
            model->move_backwards();
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

void Controller::visible(int v)
{
    if(v==GLUT_VISIBLE)
        glutIdleFunc(animate_);
    else
        glutIdleFunc(0);
}

void Controller::create_shaders_and_objects()
{
    // Create shaders
    auto phong_shader = GLShader("shaders/phong.vert",  "shaders/phong.frag");
    visualizer->add_shader(phong_shader);
    auto gauraud_shader = GLShader("shaders/gouraud.vert",  "shaders/gouraud.frag");
    visualizer->add_shader(gauraud_shader);
    
    // Create objects
    auto material = GLMaterial {{0.15f,0.15f,0.15f, 1.f}, {0.4f, 0.2f, 0.2f, 1.f}, {0.2f, 0.2f, 0.8f, 1.f}};
    auto cube = GLObject(phong_shader, material);
    std::vector<glm::vec3> data;
    for (auto pos : cube_data)
    {
        data.push_back(pos);
        data.push_back(glm::vec3(0.,1.,0.));
    }
    cube.set_data(data);
    visualizer->add_object(cube);
    
    material = GLMaterial {{0.55f,0.4f,0.5f, 1.f}, {0.2f, 0.3f, 0.4f, 1.f}, {0.2f, 0.3f, 0.4f, 1.f}};
    auto triangles = GLObject(gauraud_shader, material);
    data.clear();
    for (auto pos : triangles_data)
    {
        data.push_back(2.f*pos);
        data.push_back(glm::vec3(0.,1.,0.));
    }
    triangles.set_data(data);
    visualizer->add_object(triangles);
}
