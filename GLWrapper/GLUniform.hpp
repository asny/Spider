//
//  GLUniform.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 23/08/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Property.h"
#include "GLUtility.h"

template <class ValueType>
class GLUniform
{
public:
    
    GLUniform(GLuint location, std::shared_ptr<const Property<ValueType>> _property) : property(_property)
    {
        
    }
private:
    std::shared_ptr<const Property<ValueType>> property;
    
    
};
