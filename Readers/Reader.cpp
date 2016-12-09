//
//  OBJLoader.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "Reader.hpp"

//uses stb_image to try load files
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


tdogl::Bitmap Reader::load_bitmap(std::string filePath)
{
    int width, height, channels;
    unsigned char* pixels = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if(!pixels) throw std::runtime_error(stbi_failure_reason());
    
    tdogl::Bitmap bmp(width, height, (tdogl::Bitmap::Format)channels, pixels);
    stbi_image_free(pixels);
    return bmp;
}
