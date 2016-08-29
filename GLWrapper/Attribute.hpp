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

#include "Property.h"

namespace geogo
{
    template <class IDType, class ValueType>
    class Attribute
    {
    public:
        
        Attribute()
        {
            on_property_changed = std::bind(&Attribute::on_attribute_changed, this);
        }
        
        const Property<ValueType>& at(const IDType& id) const
        {
            return mapping.at(id);
        }
        
        const Property<ValueType>& at(const IDType* id) const
        {
            return at(*id);
        }
        
        Property<ValueType>& at(const IDType& id)
        {
            auto it = mapping.find(id);
            if (it == mapping.end())
            {
                it = mapping.insert(std::make_pair(id, Property<ValueType>(ValueType()))).first;
                it->second.listen_to(on_property_changed);
            }
            return it->second;
        }
        
        Property<ValueType>& at(const IDType* id)
        {
            return at(*id);
        }
        
        void listen_to(std::function<void()> on_attribute_changed)
        {
            subscribers.push_back(on_attribute_changed);
        }
        
    private:
        std::map<IDType, Property<ValueType>> mapping;
        std::vector<std::function<void()>> subscribers = std::vector<std::function<void()>>();
        std::function<void()> on_property_changed;
        
        void on_attribute_changed()
        {
            for(auto callback : subscribers)
            {
                callback();
            }
        }
    };
}
