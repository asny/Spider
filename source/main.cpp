//
//  main.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include <iostream>

#include "GUI.h"

int main(int argc, char** argv) {
    // insert code here...
    std::cout << "Hello, World!\n";
    GUI gui(argc, argv);
    glutMainLoop();
    return 0;
}
