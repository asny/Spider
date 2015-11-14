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

mat4 GLCamera::viewMatrix = mat4(1.);
mat4 GLCamera::projectionMatrix = mat4(1.);

GLCamera::GLCamera()
{
    // Enable states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    check_gl_error();
}

void GLCamera::set_screen_size(int width, int height)
{
    glViewport(0, 0, width, height);
    projectionMatrix = perspective(45.f, width/float(height), 0.01f, 100.f);
    check_gl_error();
}

void GLCamera::set_view(const vec3& eyePosition, const vec3& eyeDirection)
{
    viewMatrix = lookAt(eyePosition, eyePosition + eyeDirection, vec3(0., 1., 0.));
    check_gl_error();
}

void GLCamera::draw(vector<shared_ptr<GLObject>> objects)
{
    glClearColor(1., 1., 1., 0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (std::shared_ptr<GLObject> object : objects) {
        object->draw(viewMatrix, projectionMatrix);
    }
    
    check_gl_error();
}
