//
//  GLScene.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 30/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLObject.hpp"
#include "GLCamera.hpp"

#define GLFW_INCLUDE_NONE
#include <glfw3.h>

namespace oogl {
    class GLScene
    {
        GLFWwindow* gWindow;
        
        std::shared_ptr<glm::mat4> modelView = std::make_shared<glm::mat4>(1.);
        std::shared_ptr<glm::mat4> inverseModelView = std::make_shared<glm::mat4>(1.);
        std::shared_ptr<glm::mat4> modelViewProjection = std::make_shared<glm::mat4>(1.);
        
    public:
        GLScene(GLFWwindow* _gWindow, std::shared_ptr<GLCamera> _camera) : gWindow(_gWindow), camera(_camera)
        {
            
        }
        
        void add(std::shared_ptr<GLObject> object)
        {
            objects.push_back(object);
            object->use_uniform("MVMatrix", modelView);
            object->use_uniform("NMatrix", inverseModelView);
            object->use_uniform("PMatrix", camera->get_projection());
            object->use_uniform("MVPMatrix", modelViewProjection);
            object->use_uniform("eyePosition", camera->get_position());
        }
        
        void draw()
        {
            camera->pre_draw();
            
            for (std::shared_ptr<GLObject> object : objects)
            {
                *modelView = *camera->get_view() * object->get_model();
                *inverseModelView = inverseTranspose(*modelView);
                *modelViewProjection = *camera->get_projection() * (*modelView);
                
                object->draw();
            }
            
            glfwSwapBuffers(gWindow);
            
            check_gl_error();
        }
        
    private:
        std::vector<std::shared_ptr<GLObject>> objects = std::vector<std::shared_ptr<GLObject>>();
        std::shared_ptr<GLCamera> camera;
    };
}
