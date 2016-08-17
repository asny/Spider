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
        std::vector<std::function<void()>> listeners = std::vector<std::function<void()>>();
        
    public:
        Attribute()
        {
            
        }
        
        const ValueType& at(const IDType& id) const
        {
            return mapping.at(id);
        }
        
        const ValueType& at(const IDType* id) const
        {
            return at(*id);
        }
        
        void add(const IDType& id, const ValueType& value)
        {
            mapping.insert(std::pair<IDType, ValueType>(id,value));
            for(auto listener : listeners)
            {
                listener();
            }
        }
        
        void add(const IDType* id, const ValueType& value)
        {
            add(*id, value);
        }
        
        void subscribe(std::function<void()> callback)
        {
            listeners.push_back(callback);
        }
    };
}
