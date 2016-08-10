//
//  ID.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 10/08/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

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
}