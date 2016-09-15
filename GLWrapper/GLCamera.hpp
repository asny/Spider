//
//  view.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"
#include "GLObject.hpp"

namespace oogl {
    
    /**
     Represents a camera.
     */
    class GLCamera
    {
        std::shared_ptr<glm::vec3> position = std::make_shared<glm::vec3>();
        std::shared_ptr<glm::mat4> view = std::make_shared<glm::mat4>(1.);
        std::shared_ptr<glm::mat4> projection = std::make_shared<glm::mat4>(1.);
        
    public:
        
        GLCamera();
        
        /**
         Reshape the window.
         */
        void set_screen_size(int width, int height);
        
        /**
         Set the camera/eye.
         */
        void set_view(const glm::vec3& eyePosition, const glm::vec3& eyeDirection);
        
        void pre_draw();
        
        const std::shared_ptr<glm::mat4> get_view()
        {
            return view;
        }
        
        const std::shared_ptr<glm::mat4> get_projection()
        {
            return projection;
        }
        
        const std::shared_ptr<glm::vec3> get_position()
        {
            return position;
        }
        
    };
}
