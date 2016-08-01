//
//  Geometry.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 26/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Attribute.hpp"
#include "vec2.hpp"
#include "vec3.hpp"

class VertexID
{
    friend class Geometry;
    
    int id;
    
    VertexID(int _id) : id(_id)
    {
        
    }
public:
    
    friend bool operator<(const VertexID& a, const VertexID& b) { return a.id < b.id; }
    friend bool operator>(const VertexID& a, const VertexID& b) { return a.id > b.id; }
    friend bool operator<=(const VertexID& a, const VertexID& b) { return a.id <= b.id; }
    friend bool operator>=(const VertexID& a, const VertexID& b) { return a.id >= b.id; }
    friend bool operator==(const VertexID& a, const VertexID& b) { return a.id == b.id; }
    friend bool operator!=(const VertexID& a, const VertexID& b) { return a.id != b.id; }
    
    VertexID operator++(int)
    {
        id++;
        return *this;
    }
    
};

class Geometry
{
    std::vector<std::shared_ptr<Attribute<VertexID, glm::vec2>>> vec2VertexAttributes = std::vector<std::shared_ptr<Attribute<VertexID, glm::vec2>>>();
    std::vector<std::shared_ptr<Attribute<VertexID, glm::vec3>>> vec3VertexAttributes = std::vector<std::shared_ptr<Attribute<VertexID, glm::vec3>>>();
    
    int no_vertices;
    
public:
    Geometry(const std::vector<glm::vec3>& positions) : no_vertices(static_cast<int>(positions.size()))
    {
        auto attribute = std::shared_ptr<Attribute<VertexID, glm::vec3>>(new Attribute<VertexID, glm::vec3>("position"));
        for(int i = 0; i < positions.size(); i++)
        {
            attribute->add(i, positions.at(i));
        }
        vec3VertexAttributes.push_back(attribute);
    }
    
    glm::vec3 get_position(int vertexId)
    {
        return vec3VertexAttributes.front()->get_value(vertexId);
    }
    
    std::shared_ptr<Attribute<VertexID, glm::vec2>> add_vertex_attribute(std::string name, const std::vector<glm::vec2>& values)
    {
        auto attribute = std::shared_ptr<Attribute<VertexID, glm::vec2>>(new Attribute<VertexID, glm::vec2>(name));
        for(int i = 0; i < values.size(); i++)
        {
            attribute->add(VertexID(i), values.at(i));
        }
        vec2VertexAttributes.push_back(attribute);
        return attribute;
    }
    
    std::shared_ptr<Attribute<VertexID, glm::vec3>> add_vertex_attribute(std::string name, const std::vector<glm::vec3>& values)
    {
        auto attribute = std::shared_ptr<Attribute<VertexID, glm::vec3>>(new Attribute<VertexID, glm::vec3>(name));
        for(int i = 0; i < values.size(); i++)
        {
            attribute->add(i, values.at(i));
        }
        vec3VertexAttributes.push_back(attribute);
        return attribute;
    }
    
    std::vector<std::shared_ptr<Attribute<VertexID, glm::vec2>>> get_vec2_vertex_attributes()
    {
        return vec2VertexAttributes;
    }
    
    std::vector<std::shared_ptr<Attribute<VertexID, glm::vec3>>> get_vec3_vertex_attributes()
    {
        return vec3VertexAttributes;
    }
    
    VertexID vertices_begin()
    {
        return VertexID(0);
    }
    
    VertexID vertices_end()
    {
        return VertexID(no_vertices);
    }
};