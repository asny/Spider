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
        class Value
        {
            friend class Attribute;
            
            Attribute<IDType, ValueType>* attribute;
            IDType id;
            ValueType value;
            
            Value(ValueType _value) : value(_value)
            {
                
            }
            
            Value(Attribute<IDType, ValueType>* _attribute, IDType _id) : attribute(_attribute), id(_id)
            {
                
            }
            
            Value(Attribute<IDType, ValueType>* _attribute, IDType _id, ValueType _value) : attribute(_attribute), id(_id), value(_value)
            {
                
            }
            
        public:
            operator ValueType() const
            {
                return value;
            };
            
            void operator=(const ValueType& _value)
            {
                value = _value;
                attribute->add(id, _value);
            }
        };
        
        Attribute()
        {
            
        }
        
        const Value at(const IDType& id) const
        {
            return Value(mapping.at(id));
        }
        
        const Value at(const IDType* id) const
        {
            return at(*id);
        }
        
        Value at(const IDType& id)
        {
            auto it = mapping.find(id);
            if (it != mapping.end())
            {
                return Value(this, id, it->second);
            }
            return Value(this, id);
        }
        
        Value at(const IDType* id)
        {
            return at(*id);
        }
        
        void subscribe(std::function<void()> callback)
        {
            listeners.push_back(callback);
        }
        
    private:
        std::map<IDType, ValueType> mapping;
        std::vector<std::function<void()>> listeners = std::vector<std::function<void()>>();
        
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
    };
}
