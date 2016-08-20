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
    public:
        GLScene(GLFWwindow* _gWindow) : gWindow(_gWindow)
        {
            
        }
        
        void add(std::shared_ptr<GLObject> object)
        {
            objects.push_back(object);
        }
        
        void add(std::shared_ptr<GLCamera> camera)
        {
            cameras.push_back(camera);
        }
        
        void draw()
        {
            for(auto camera : cameras)
            {
                camera->draw(objects);
                
                glfwSwapBuffers(gWindow);
            }
        }
        
    private:
        std::vector<std::shared_ptr<GLObject>> objects = std::vector<std::shared_ptr<GLObject>>();
        std::vector<std::shared_ptr<GLCamera>> cameras = std::vector<std::shared_ptr<GLCamera>>();
    };
}
