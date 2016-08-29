//
//  Property.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 23/08/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <functional>
#include <vector>

template <class T>
class Property
{
    std::vector<std::function<void()>> subscribers;
    T value;
public:
    Property(T&& _value) : value(_value)
    {
        
    }
    
    void listen_to(std::function<void()> on_property_changed)
    {
        subscribers.push_back(on_property_changed);
    }
    
    operator const T&() const
    {
        return value;
    };
    
    void operator=(const T& _value)
    {
        value = _value;
        for(auto on_property_changed : subscribers)
        {
            on_property_changed();
        }
    }
    
    void operator=(T&& _value)
    {
        value = _value;
        for(auto on_property_changed : subscribers)
        {
            on_property_changed();
        }
    }
};