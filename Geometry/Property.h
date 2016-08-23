//
//  Property.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 23/08/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include <functional>

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