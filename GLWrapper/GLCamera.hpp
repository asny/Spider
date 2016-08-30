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
    class GLCamera {
        glm::mat4 view = glm::mat4(1.);
        glm::mat4 projection = glm::mat4(1.);
        
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
        
        const glm::mat4& get_view()
        {
            return view;
        }
        
        const glm::mat4& get_projection()
        {
            return projection;
        }
    };
}
