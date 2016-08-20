//
//  Geometry.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 26/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#include "Geometry.hpp"

using namespace geogo;
using namespace glm;

std::shared_ptr<Geometry> Geometry::create_box(bool view_from_inside)
{
    auto geometry = std::shared_ptr<Geometry>(new Geometry());
    
    auto vertexNNP = geometry->create_vertex(vec3(-1.0, -1.0,  1.0));
    auto vertexPPP = geometry->create_vertex(vec3(1.0, 1.0,  1.0));
    auto vertexPNP = geometry->create_vertex(vec3(1.0, -1.0,  1.0));
    auto vertexNPP = geometry->create_vertex(vec3(-1.0, 1.0,  1.0));
    
    auto vertexNNN = geometry->create_vertex(vec3(-1.0, -1.0,  -1.0));
    auto vertexPPN = geometry->create_vertex(vec3(1.0, 1.0,  -1.0));
    auto vertexPNN = geometry->create_vertex(vec3(1.0, -1.0,  -1.0));
    auto vertexNPN = geometry->create_vertex(vec3(-1.0, 1.0,  -1.0));
    
    if(view_from_inside)
    {
        // Front
        geometry->create_face(vertexNNP, vertexPPP, vertexPNP);
        geometry->create_face(vertexPPP, vertexNNP, vertexNPP);
        
        // Top
        geometry->create_face(vertexNPP, vertexPPN, vertexPPP);
        geometry->create_face(vertexPPN, vertexNPP, vertexNPN);
        
        // Back
        geometry->create_face(vertexPNN, vertexNPN, vertexNNN);
        geometry->create_face(vertexNPN, vertexPNN, vertexPPN);
        
        // Bottom
        geometry->create_face(vertexNNN, vertexPNP, vertexPNN);
        geometry->create_face(vertexPNP, vertexNNN, vertexNNP);
        
        // Left
        geometry->create_face(vertexNNN, vertexNPP, vertexNNP);
        geometry->create_face(vertexNPP, vertexNNN, vertexNPN);
        
        // Right
        geometry->create_face(vertexPNP, vertexPPN, vertexPNN);
        geometry->create_face(vertexPPN, vertexPNP, vertexPPP);
    }
    else {
        // Front
        geometry->create_face(vertexNNP, vertexPNP, vertexPPP);
        geometry->create_face(vertexPPP, vertexNPP, vertexNNP);
        
        // Top
        geometry->create_face(vertexNPP, vertexPPP, vertexPPN);
        geometry->create_face(vertexPPN, vertexNPN, vertexNPP);
        
        // Back
        geometry->create_face(vertexPNN, vertexNNN, vertexNPN);
        geometry->create_face(vertexNPN, vertexPPN, vertexPNN);
        
        // Bottom
        geometry->create_face(vertexNNN, vertexPNN, vertexPNP);
        geometry->create_face(vertexPNP, vertexNNP, vertexNNN);
        
        // Left
        geometry->create_face(vertexNNN, vertexNNP, vertexNPP);
        geometry->create_face(vertexNPP, vertexNPN, vertexNNN);
        
        // Right
        geometry->create_face(vertexPNP, vertexPNN, vertexPPN);
        geometry->create_face(vertexPPN, vertexPPP, vertexPNP);
    }
    return geometry;
}