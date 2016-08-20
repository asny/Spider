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

#include "Geometry.hpp"
#include "Bitmap.h"

class Reader {
    
public:
    /**
     Loads an obj file and constructs the corresponding Geometry.
     */
    static void load_obj(std::string file_path, geogo::Geometry& geometry, geogo::Attribute<geogo::VertexID, glm::vec2>& uv_coordinates, geogo::Attribute<geogo::VertexID, glm::vec3>& normals);
    
    static void load_obj(std::string file_path, geogo::Geometry& geometry, geogo::Attribute<geogo::VertexID, glm::vec3>& normals)
    {
        geogo::Attribute<geogo::VertexID, glm::vec2> uv_coordinates;
        load_obj(file_path, geometry, uv_coordinates, normals);
    }
    
    static void load_obj(std::string file_path, geogo::Geometry& geometry, geogo::Attribute<geogo::VertexID, glm::vec2>& uv_coordinates)
    {
        geogo::Attribute<geogo::VertexID, glm::vec3> normals;
        load_obj(file_path, geometry, uv_coordinates, normals);
    }
    
    static void load_obj(std::string file_path, geogo::Geometry& geometry)
    {
        geogo::Attribute<geogo::VertexID, glm::vec2> uv_coordinates;
        geogo::Attribute<geogo::VertexID, glm::vec3> normals;
        load_obj(file_path, geometry, uv_coordinates, normals);
    }
    
    /**
     Tries to load the given file into an instance of the Bitmap class.
     */
    static tdogl::Bitmap load_bitmap(std::string filePath);
    
    // Create a NULL-terminated string by reading the provided file
    static char* read_shader_source(std::string filePath);
};