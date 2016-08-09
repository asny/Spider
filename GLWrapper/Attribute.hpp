//
//  Attribute.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 30/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <iostream>
#include <map>

template <class IDType, class ValueType>
class Attribute
{
    std::map<IDType, ValueType> mapping;
    
public:
    Attribute()
    {
        
    }
    
    void add(IDType id, ValueType value)
    {
        mapping[id] = value;
    }
    
    int get_size()
    {
        return static_cast<int>(mapping.size());
    }
    
    ValueType get_value(IDType id)
    {
        return mapping.at(id);
    }
    
    
};
