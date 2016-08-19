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
    template <class ValueType>
    class Value
    {
        std::function<void()> on_value_changed;
        ValueType value;
    public:
        Value(ValueType&& _value, std::function<void()> _on_value_changed) : value(_value), on_value_changed(_on_value_changed)
        {
            
        }
        
        operator const ValueType&() const
        {
            return value;
        };
        
        void operator=(const ValueType& _value)
        {
            value = _value;
            on_value_changed();
        }
        
        void operator=(ValueType&& _value)
        {
            value = _value;
            on_value_changed();
        }
    };
    
    template <class IDType, class ValueType>
    class Attribute
    {
    public:
        
        Attribute()
        {
            on_value_changed = std::bind(&Attribute::on_attribute_changed, this);
        }
        
        const Value<ValueType>& at(const IDType& id) const
        {
            return mapping.at(id);
        }
        
        const Value<ValueType>& at(const IDType* id) const
        {
            return at(*id);
        }
        
        Value<ValueType>& at(const IDType& id)
        {
            auto it = mapping.find(id);
            if (it == mapping.end())
            {
                it = mapping.insert(std::pair<IDType, Value<ValueType>>(id, Value<ValueType>(ValueType(), on_value_changed))).first;
            }
            return it->second;
        }
        
        Value<ValueType>& at(const IDType* id)
        {
            return at(*id);
        }
        
        void subscribe_to(std::function<void()> on_attribute_changed)
        {
            subscribers.push_back(on_attribute_changed);
        }
        
    private:
        std::map<IDType, Value<ValueType>> mapping;
        std::vector<std::function<void()>> subscribers = std::vector<std::function<void()>>();
        std::function<void()> on_value_changed;
        
        void on_attribute_changed()
        {
            for(auto callback : subscribers)
            {
                callback();
            }
        }
    };
}
