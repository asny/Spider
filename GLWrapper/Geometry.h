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
        
        EdgeID* start_edge = nullptr;
        EdgeID* end_edge = nullptr;
        
        FaceID* start_face = nullptr;
        FaceID* end_face = nullptr;
        
        int no_vertices = 0;
        int no_edges = 0;
        int no_faces = 0;
        
        std::shared_ptr<Attribute<VertexID, glm::vec3>> position_attribute = std::make_shared<Attribute<VertexID, glm::vec3>>();
        
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
        
        EdgeID* create_edge(VertexID* vertex1, VertexID* vertex2)
        {
            end_edge = new EdgeID(no_edges, end_edge, vertex1, vertex2);
            no_edges++;
            if(!start_edge)
                start_edge = end_edge;
            return end_edge;
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
        
        VertexID* vertices_begin()
        {
            return start_vertex;
        }
        
        VertexID* vertices_end()
        {
            return nullptr;
        }
        
        EdgeID* edges_begin()
        {
            return start_edge;
        }
        
        EdgeID* edges_end()
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
        
        int get_no_edges()
        {
            return no_edges;
        }
        
        int get_no_faces()
        {
            return no_faces;
        }
    };
}
