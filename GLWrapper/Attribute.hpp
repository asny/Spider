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
#include <vector>

namespace geogo
{
    template <class IDType, class ValueType>
    class Attribute
    {
        std::map<IDType, ValueType> mapping;
        std::vector<std::function<void(Attribute<IDType, ValueType>*)>> listeners = std::vector<std::function<void(Attribute<IDType, ValueType>*)>>();
        
    public:
        Attribute()
        {
            
        }
        
        void add(IDType id, ValueType value)
        {
            mapping[id] = value;
            for(auto listener : listeners)
            {
                listener(this);
            }
        }
        
        ValueType get(IDType id)
        {
            return mapping.at(id);
        }
        
        void subscribe(std::function<void(Attribute<IDType, ValueType>*)> callback)
        {
            listeners.push_back(callback);
        }
    };
}
