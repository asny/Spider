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
        ID(int _id) : id(_id)
        {
            
        }
    public:
        
        friend bool operator<(const ID& a, const ID& b) { return a.id < b.id; }
        friend bool operator>(const ID& a, const ID& b) { return a.id > b.id; }
        friend bool operator<=(const ID& a, const ID& b) { return a.id <= b.id; }
        friend bool operator>=(const ID& a, const ID& b) { return a.id >= b.id; }
        friend bool operator==(const ID& a, const ID& b) { return a.id == b.id; }
        friend bool operator!=(const ID& a, const ID& b) { return a.id != b.id; }
        
        ID operator++(int)
        {
            id++;
            return *this;
        }
        
    };
    
    class VertexID : public ID
    {
        friend class Geometry;
        
        VertexID(int id) : ID(id)
        {
            
        }
    };
    
    class FaceID : public ID
    {
        friend class Geometry;
        
        std::vector<VertexID> vertices;
        
        FaceID(int id, std::vector<VertexID> _vertices) : ID(id), vertices(_vertices)
        {
            
        }
    };
}