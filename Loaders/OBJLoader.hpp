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

class OBJLoader {
    
public:
    bool static load(const char * path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3> & out_normals);
    bool static load(const char * path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs);
};