//
//  view.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"

namespace oogl {
    
    /**
     A wrapper for OpenGL.
     */
    class GLWrapper {
        
    public:
        
        static void cull_backface(bool enable = true)
        {
            if(enable)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }
            else {
                glDisable(GL_CULL_FACE);
            }
        }
        
        static void initialize();
        
        /**
         Reshape the window.
         */
        static void set_screen_size(int width, int height);
        
        /**
         Set the camera/eye.
         */
        static void set_view(const glm::vec3& eyePosition, const glm::vec3& eyeDirection);
        
        /**
         Initialize drawing the objects. Should be called before any draw calls to a GLObject.
         */
        static void initialize_draw();
    };
}
