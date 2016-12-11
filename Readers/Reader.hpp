//
//  OBJLoader.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Bitmap.h"

class Reader {
    
public:
    
    /**
     Tries to load the given file into an instance of the Bitmap class.
     */
    static tdogl::Bitmap load_bitmap(std::string filePath);
};
