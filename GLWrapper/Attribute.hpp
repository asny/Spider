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
    template <class T>
    class Property
    {
        std::function<void()> on_property_changed;
        T value;
    public:
        Property(T&& _value, std::function<void()> _on_property_changed) : value(_value), on_property_changed(_on_property_changed)
        {
            
        }
        
        operator const T&() const
        {
            return value;
        };
        
        void operator=(const T& _value)
        {
            value = _value;
            on_property_changed();
        }
        
        void operator=(T&& _value)
        {
            value = _value;
            on_property_changed();
        }
    };
    
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
                it = mapping.insert(std::pair<IDType, Property<ValueType>>(id, Property<ValueType>(ValueType(), on_property_changed))).first;
            }
            return it->second;
        }
        
        Property<ValueType>& at(const IDType* id)
        {
            return at(*id);
        }
        
        void subscribe_to(std::function<void()> on_attribute_changed)
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
