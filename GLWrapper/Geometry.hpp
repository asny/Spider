//
//  Geometry.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 26/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Attribute.hpp"
#include "vec3.hpp"

class Geometry
{
    std::vector<std::shared_ptr<Attribute<int, glm::vec3>>> vertexAttributes = std::vector<std::shared_ptr<Attribute<int, glm::vec3>>>();
    
public:
    Geometry(std::shared_ptr<Attribute<int, glm::vec3>> _vertexPositions)
    {
        vertexAttributes.push_back(_vertexPositions);
    }
    
    Geometry(const std::vector<glm::vec3>& positions)
    {
        auto attribute = std::shared_ptr<Attribute<int, glm::vec3>>(new Attribute<int, glm::vec3>("position"));
        for(int i = 0; i < positions.size(); i++)
        {
            attribute->add(i, positions.at(i));
        }
        vertexAttributes.push_back(attribute);
    }
    
    glm::vec3 get_vertex_position(int vertexId)
    {
        return vertexAttributes.front()->get_value(vertexId);
    }
    
    std::vector<std::shared_ptr<Attribute<int, glm::vec3>>> get_vec3_vertex_attributes()
    {
        return vertexAttributes;
    }
    
    // TODO: Implement iterator
    
};