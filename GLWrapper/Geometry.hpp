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
    std::vector<std::shared_ptr<Attribute<int, glm::vec2>>> vec2VertexAttributes = std::vector<std::shared_ptr<Attribute<int, glm::vec2>>>();
    std::vector<std::shared_ptr<Attribute<int, glm::vec3>>> vec3VertexAttributes = std::vector<std::shared_ptr<Attribute<int, glm::vec3>>>();
    
public:
    Geometry()
    {
        
    }
    
    std::shared_ptr<Attribute<int, glm::vec2>> add_vertex_attribute(std::string name, const std::vector<glm::vec2>& values)
    {
        auto attribute = std::shared_ptr<Attribute<int, glm::vec2>>(new Attribute<int, glm::vec2>(name));
        for(int i = 0; i < values.size(); i++)
        {
            attribute->add(i, values.at(i));
        }
        vec2VertexAttributes.push_back(attribute);
        return attribute;
    }
    
    std::shared_ptr<Attribute<int, glm::vec3>> add_vertex_attribute(std::string name, const std::vector<glm::vec3>& values)
    {
        auto attribute = std::shared_ptr<Attribute<int, glm::vec3>>(new Attribute<int, glm::vec3>(name));
        for(int i = 0; i < values.size(); i++)
        {
            attribute->add(i, values.at(i));
        }
        vec3VertexAttributes.push_back(attribute);
        return attribute;
    }
    
    std::vector<std::shared_ptr<Attribute<int, glm::vec2>>> get_vec2_vertex_attributes()
    {
        return vec2VertexAttributes;
    }
    
    std::vector<std::shared_ptr<Attribute<int, glm::vec3>>> get_vec3_vertex_attributes()
    {
        return vec3VertexAttributes;
    }
    
    // TODO: Implement iterator
    
};