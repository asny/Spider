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
    
    class FaceID : public ID
    {
        friend class Geometry;
        
        std::vector<VertexID> vertices;
        
        FaceID(int id, ID* previous, std::vector<VertexID> _vertices) : ID(id, previous), vertices(_vertices)
        {
            
        }
        
    public:
        FaceID* next()
        {
            return static_cast<FaceID*>(ID::next);
        }
    };
}