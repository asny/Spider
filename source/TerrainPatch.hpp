//
//  Created by Asger Nyman Christiansen on 06/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "Terrain.hpp"

class TerrainPatch
{
    Terrain terrain = Terrain();
    
public:
    TerrainPatch()
    {
        
    }
    
    Terrain& get_terrain()
    {
        return terrain;
    }
    
};
