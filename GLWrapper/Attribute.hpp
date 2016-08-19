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
    public:
        class ConstValue
        {
            friend class Attribute;
            const ValueType& const_value;
            
            ConstValue(const ValueType& value) : const_value(value)
            {
                
            }
            
        public:
            operator const ValueType&() const
            {
                return const_value;
            };
        };
        
        class Value : public ConstValue
        {
            friend class Attribute;
            Attribute<IDType, ValueType>* attribute;
            ValueType& value;
            
            Value(Attribute<IDType, ValueType>* _attribute, ValueType& _value) : ConstValue(_value), attribute(_attribute), value(_value)
            {
                
            }
            
        public:
            void operator=(const ValueType& _value)
            {
                value = _value;
                attribute->on_attribute_changed();
            }
            
            void operator=(ValueType&& _value)
            {
                value = _value;
                attribute->on_attribute_changed();
            }
        };
        
        Attribute()
        {
            
        }
        
        ConstValue at(const IDType& id) const
        {
            return ConstValue(mapping.at(id));
        }
        
        ConstValue at(const IDType* id) const
        {
            return at(*id);
        }
        
        Value at(const IDType& id)
        {
            auto it = mapping.find(id);
            if (it == mapping.end())
            {
                it = mapping.insert(std::pair<IDType, ValueType>(id, ValueType())).first;
            }
            return Value(this, it->second);
        }
        
        Value at(const IDType* id)
        {
            return at(*id);
        }
        
        void subscribe_to(std::function<void()> on_attribute_changed)
        {
            subscribers.push_back(on_attribute_changed);
        }
        
    private:
        std::map<IDType, ValueType> mapping;
        std::vector<std::function<void()>> subscribers = std::vector<std::function<void()>>();
        
        void on_attribute_changed()
        {
            for(auto on_attribute_changed : subscribers)
            {
                on_attribute_changed();
            }
        }
    };
}
