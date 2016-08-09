//
//  Geometry.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 26/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Attribute.hpp"
#include <vector>
#include "vec2.hpp"
#include "vec3.hpp"

namespace geogo
{
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
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec2>>> vec2VertexAttributes =
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec2>>>();
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec3>>> vec3VertexAttributes =
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec3>>>();
        
        int no_vertices = 0;
        
    public:
        Geometry()
        {
            
        }
        
        Geometry(const std::vector<glm::vec3>& positions)
        {
            add_vertex_attribute("position", positions);
        }
        
        glm::vec3 get_position(int vertexId)
        {
            return vec3VertexAttributes.at("position")->get_value(vertexId);
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec2>> add_vertex_attribute(std::string name, const std::vector<glm::vec2>& values)
        {
            auto attribute = get_vec2_vertex_attribute(name);
            if(!attribute)
            {
                // Lazy construction
                attribute = std::shared_ptr<Attribute<VertexID, glm::vec2>>(new Attribute<VertexID, glm::vec2>());
                for(int i = 0; i < values.size(); i++)
                {
                    attribute->add(i, values.at(i));
                }
                vec2VertexAttributes.insert(std::pair<std::string, std::shared_ptr<Attribute<VertexID, glm::vec2>>>(name, attribute));
            }
            else
            {
                for(int i = 0; i < values.size(); i++)
                {
                    attribute->add(i, values.at(i));
                }
            }
            return attribute;
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec3>> add_vertex_attribute(std::string name, const std::vector<glm::vec3>& values)
        {
            // TODO: Do not infer the number of vertices from this.
            if(name == "position")
            {
                no_vertices = static_cast<int>(values.size());
            }
            auto attribute = get_vec3_vertex_attribute(name);
            if(!attribute)
            {
                // Lazy construction
                attribute = std::shared_ptr<Attribute<VertexID, glm::vec3>>(new Attribute<VertexID, glm::vec3>());
                for(int i = 0; i < values.size(); i++)
                {
                    attribute->add(i, values.at(i));
                }
                vec3VertexAttributes.insert(std::pair<std::string, std::shared_ptr<Attribute<VertexID, glm::vec3>>>(name, attribute));
            }
            else
            {
                for(int i = 0; i < values.size(); i++)
                {
                    attribute->add(i, values.at(i));
                }
            }
            return attribute;
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec2>> get_vec2_vertex_attribute(std::string name)
        {
            auto result = vec2VertexAttributes.find(name);
            if(result != vec2VertexAttributes.end())
            {
                return result->second;
            }
            return nullptr;
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec3>> get_vec3_vertex_attribute(std::string name)
        {
            auto result = vec3VertexAttributes.find(name);
            if(result != vec3VertexAttributes.end())
            {
                return result->second;
            }
            return nullptr;
        }
        
        VertexID vertices_begin()
        {
            return VertexID(0);
        }
        
        VertexID vertices_end()
        {
            return VertexID(no_vertices);
        }
        
        int get_no_vertices()
        {
            return no_vertices;
        }
    };
    
    class Lines : Geometry
    {
        
    };
    
    class TriangleMesh : Geometry
    {
        
    };
}
