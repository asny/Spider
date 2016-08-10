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
            auto attribute = get_vec3_vertex_attribute("position");
            
            for (auto position : positions) {
                auto vertex = create_vertex();
                attribute->add(*vertex, position);
            }
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
