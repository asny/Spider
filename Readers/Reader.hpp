//
//  OBJLoader.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <vector>
#include "vec2.hpp"
#include "vec3.hpp"

#include "Bitmap.h"

class Reader {
    
public:
    bool static load_obj(std::string filePath, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3> & out_normals);
    bool static load_obj(std::string filePath, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs);
    
    
    /**
     Tries to load the given file into a tdogl::Bitmap.
     */
    tdogl::Bitmap static load_bitmap(std::string filePath);
};