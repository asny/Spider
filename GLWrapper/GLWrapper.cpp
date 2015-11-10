//
//  view.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GLWrapper.h"
#include "GLObject.hpp"

using namespace oogl;
using namespace std;
using namespace glm;

void GLWrapper::initialize()
{
    // Enable states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    check_gl_error();
}

void GLWrapper::set_screen_size(int width, int height)
{
    glViewport(0, 0, width, height);
    GLObject::set_projection_matrix(perspective(45.f, width/float(height), 0.01f, 100.f));
    check_gl_error();
}

void GLWrapper::set_view(const vec3& eyePosition, const vec3& eyeDirection)
{
    GLObject::set_view_matrix(lookAt(eyePosition, eyePosition + eyeDirection, vec3(0., 1., 0.)));
    check_gl_error();
}

void GLWrapper::initialize_draw()
{
    glClearColor(1., 1., 1., 0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    check_gl_error();
}
