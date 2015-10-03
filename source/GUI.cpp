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

GUI::GUI(int &argc, char** argv)
{
    instance = this;
    
    glutInit(&argc, argv);
#ifdef _WIN32
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
#endif
    glutCreateWindow("Spider");
    
    glutDisplayFunc(display_);
    glutKeyboardFunc(keyboard_);
    glutIgnoreKeyRepeat(true);
    glutVisibilityFunc(visible_);
    glutReshapeFunc(reshape_);
    glutIdleFunc(animate_);
    
#ifndef __APPLE__
    glewExperimental = GL_TRUE;  // See http://www.opengl.org/wiki/OpenGL_Loading_Library
    GLint GlewInitResult = glewInit();
    if (GlewInitResult != GLEW_OK) {
        printf("ERROR: %s\n", glewGetErrorString(GlewInitResult));
    }
    check_gl_error(); // Catches a GL_INVALID_ENUM error. See http://www.opengl.org/wiki/OpenGL_Loading_Library
#endif
    
    glutReshapeWindow(WIN_SIZE_X, WIN_SIZE_Y);
    
    visualizer = std::unique_ptr<Visualizer>(new Visualizer(light_pos));
    visualizer->update();
    glutPostRedisplay();
    
    check_gl_error();
    glutMainLoop();
}

void GUI::display()
{
    if (glutGet(GLUT_WINDOW_WIDTH) != WIN_SIZE_X || glutGet(GLUT_WINDOW_HEIGHT) != WIN_SIZE_Y) {
        return;
    }
    GLfloat timeValue = glutGet(GLUT_ELAPSED_TIME)*0.0002;
    glm::vec3 ep( eye_pos[0] * sinf(timeValue), eye_pos[1] * cosf(timeValue) , eye_pos[2] * cosf(timeValue));
    visualizer->set_view_position(ep);
    visualizer->draw();
    glutSwapBuffers();
    check_gl_error();
}

void GUI::reshape(int width, int height)
{
    WIN_SIZE_X = width;
    WIN_SIZE_Y = height;
    visualizer->reshape(width, height);
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