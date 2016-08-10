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
        
        FaceID(int id) : ID(id)
        {
            
        }
    };
}