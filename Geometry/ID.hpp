//
//  ID.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 10/08/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once
#include <vector>

namespace geogo
{
    class ID
    {
        int id;
        
    protected:
        ID* previous = nullptr;
        ID* next = nullptr;
        
        ID(int _id, ID* _previous) : id(_id), previous(_previous)
        {
            if(previous)
                _previous->next = this;
        }
        
    public:
        
        friend bool operator<(const ID& a, const ID& b) { return a.id < b.id; }
        friend bool operator>(const ID& a, const ID& b) { return a.id > b.id; }
        friend bool operator<=(const ID& a, const ID& b) { return a.id <= b.id; }
        friend bool operator>=(const ID& a, const ID& b) { return a.id >= b.id; }
        friend bool operator==(const ID& a, const ID& b) { return a.id == b.id; }
        friend bool operator!=(const ID& a, const ID& b) { return a.id != b.id; }
    };
    
    class VertexID : public ID
    {
        friend class Geometry;
        
        VertexID(int id, ID* previous) : ID(id, previous)
        {
            
        }
        
    public:
        VertexID* next()
        {
            return static_cast<VertexID*>(ID::next);
        }
    };
    
    class EdgeID : public ID
    {
        friend class Geometry;
        
        VertexID* vertex1;
        VertexID* vertex2;
        
        EdgeID(int id, EdgeID* previous, VertexID* _vertex1, VertexID* _vertex2)
            : ID(id, previous), vertex1(_vertex1), vertex2(_vertex2)
        {
            
        }
        
    public:
        EdgeID* next()
        {
            return static_cast<EdgeID*>(ID::next);
        }
        
        VertexID* v1()
        {
            return vertex1;
        }
        
        VertexID* v2()
        {
            return vertex2;
        }
    };
    
    class FaceID : public ID
    {
        friend class Geometry;
        
        VertexID* vertex1;
        VertexID* vertex2;
        VertexID* vertex3;
        
        FaceID(int id, FaceID* previous, VertexID* _vertex1, VertexID* _vertex2, VertexID* _vertex3)
        : ID(id, previous), vertex1(_vertex1), vertex2(_vertex2), vertex3(_vertex3)
        {
            
        }
        
    public:
        FaceID* next()
        {
            return static_cast<FaceID*>(ID::next);
        }
        
        VertexID* v1()
        {
            return vertex1;
        }
        
        VertexID* v2()
        {
            return vertex2;
        }
        
        VertexID* v3()
        {
            return vertex3;
        }
    };
}