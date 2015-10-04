//
//  main.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include <iostream>

#include "controller.h"
#include "model.hpp"

int main(int argc, char** argv)
{
    auto model = std::shared_ptr<Model>(new Model());
    Controller controller(model, argc, argv);
    return 0;
}
