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
#include "ID.hpp"

namespace geogo
{
    
    class Geometry
    {
        VertexID* start_vertex = nullptr;
        VertexID* end_vertex = nullptr;
        
        FaceID* start_face = nullptr;
        FaceID* end_face = nullptr;
        
        int no_vertices = 0;
        int no_faces = 0;
        
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec2>>> vec2VertexAttributes =
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec2>>>();
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec3>>> vec3VertexAttributes =
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec3>>>();
        
    protected:
        FaceID* create_face(std::vector<VertexID> vertices)
        {
            end_face = new FaceID(no_faces, end_face, vertices);
            no_faces++;
            return end_face;
        }
        
    public:
        Geometry()
        {
            
        }
        
        Geometry(const std::vector<glm::vec3>& positions)
        {
            // TODO: Should be created outside of Geometry.
            for (auto position : positions) {
                create_vertex();
            }
            
            add_vertex_attribute("position", positions);
        }
        
        ~Geometry()
        {
            for(auto vertex = vertices_begin(); vertex != vertices_end(); vertex = vertex->next())
            {
                delete vertex;
            }
            for(auto face = faces_begin(); face != faces_end(); face = face->next())
            {
                delete face;
            }
        }
        
        VertexID* create_vertex()
        {
            end_vertex = new VertexID(no_vertices, end_vertex);
            no_vertices++;
            if(!start_vertex)
                start_vertex = end_vertex;
            return end_vertex;
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec2>> add_vertex_attribute(std::string name, const std::vector<glm::vec2>& values)
        {
            auto attribute = get_vec2_vertex_attribute(name);
            if(!attribute)
            {
                // Lazy construction
                attribute = std::shared_ptr<Attribute<VertexID, glm::vec2>>(new Attribute<VertexID, glm::vec2>());
                vec2VertexAttributes.insert(std::pair<std::string, std::shared_ptr<Attribute<VertexID, glm::vec2>>>(name, attribute));
            }
            int i = 0;
            for(auto vertex = vertices_begin(); vertex != vertices_end(); vertex = vertex->next())
            {
                attribute->add(*vertex, values.at(i));
                i++;
            }
            return attribute;
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec3>> add_vertex_attribute(std::string name, const std::vector<glm::vec3>& values)
        {
            auto attribute = get_vec3_vertex_attribute(name);
            if(!attribute)
            {
                // Lazy construction
                attribute = std::shared_ptr<Attribute<VertexID, glm::vec3>>(new Attribute<VertexID, glm::vec3>());
                vec3VertexAttributes.insert(std::pair<std::string, std::shared_ptr<Attribute<VertexID, glm::vec3>>>(name, attribute));
            }
            int i = 0;
            for(auto vertex = vertices_begin(); vertex != vertices_end(); vertex = vertex->next())
            {
                attribute->add(*vertex, values.at(i));
                i++;
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
        
        VertexID* vertices_begin()
        {
            return start_vertex;
        }
        
        VertexID* vertices_end()
        {
            return nullptr;
        }
        
        int get_no_vertices()
        {
            return no_vertices;
        }
        
        FaceID* faces_begin()
        {
            return start_face;
        }
        
        FaceID* faces_end()
        {
            return end_face;
        }
    };
    
    class Lines : Geometry
    {
        
    };
    
    class TriangleMesh : Geometry
    {
    public:
        
        FaceID* create_face(VertexID vertex1, VertexID vertex2, VertexID vertex3)
        {
            return Geometry::create_face({vertex1, vertex2, vertex3});
        }
    };
}
