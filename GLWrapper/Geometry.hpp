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
        
        std::shared_ptr<Attribute<VertexID, glm::vec3>> position_attribute = std::shared_ptr<Attribute<VertexID, glm::vec3>>(new Attribute<VertexID, glm::vec3>());
        
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec2>>> vec2VertexAttributes =
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec2>>>();
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec3>>> vec3VertexAttributes =
        std::map<std::string, std::shared_ptr<Attribute<VertexID, glm::vec3>>>();
        
    public:
        Geometry()
        {
            
        }
        
        ~Geometry()
        {
            for(auto vertex = vertices_begin(); vertex != vertices_end(); )
            {
                auto temp = vertex->next();
                delete vertex;
                vertex = temp;
            }
            for(auto face = faces_begin(); face != faces_end(); )
            {
                auto temp = face->next();
                delete face;
                face = temp;
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
        
        VertexID* create_vertex(const glm::vec3& position)
        {
            auto vertex = create_vertex();
            position_attribute->at(vertex) = position;
            return vertex;
        }
        
        FaceID* create_face(VertexID* vertex1, VertexID* vertex2, VertexID* vertex3)
        {
            end_face = new FaceID(no_faces, end_face, vertex1, vertex2, vertex3);
            no_faces++;
            if(!start_face)
                start_face = end_face;
            return end_face;
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec3>> position()
        {
            return position_attribute;
        }
        
        std::shared_ptr<const Attribute<VertexID, glm::vec3>> position() const
        {
            return position_attribute;
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec2>> get_vec2_vertex_attribute(std::string name)
        {
            auto result = vec2VertexAttributes.find(name);
            if(result == vec2VertexAttributes.end())
            {
                // Lazy construction
                auto attribute = std::shared_ptr<Attribute<VertexID, glm::vec2>>(new Attribute<VertexID, glm::vec2>());
                vec2VertexAttributes[name] = attribute;
                return attribute;
            }
            return result->second;
        }
        
        std::shared_ptr<Attribute<VertexID, glm::vec3>> get_vec3_vertex_attribute(std::string name)
        {
            if(name == "position")
            {
                return position_attribute;
            }
            
            auto result = vec3VertexAttributes.find(name);
            if(result == vec3VertexAttributes.end())
            {
                // Lazy construction
                auto attribute = std::shared_ptr<Attribute<VertexID, glm::vec3>>(new Attribute<VertexID, glm::vec3>());
                vec3VertexAttributes[name] = attribute;
                return attribute;
            }
            return result->second;
        }
        
        VertexID* vertices_begin()
        {
            return start_vertex;
        }
        
        VertexID* vertices_end()
        {
            return nullptr;
        }
        
        FaceID* faces_begin()
        {
            return start_face;
        }
        
        FaceID* faces_end()
        {
            return nullptr;
        }
        
        int get_no_vertices()
        {
            return no_vertices;
        }
        
        int get_no_faces()
        {
            return no_faces;
        }
        
        static std::shared_ptr<Geometry> create_box(bool view_from_inside);
    };
}
