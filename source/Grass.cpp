//
//  Created by Asger Nyman Christiansen on 03/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#include "Grass.hpp"
#include "Random.h"

using namespace std;
using namespace glm;
using namespace gle;

void Grass::initialize(Terrain& patch)
{
    int NO_GRASS_STRAW = NO_GRASS_STRAW_PER_UNIT * Terrain::SIZE * Terrain::SIZE;
    
    auto edge = geometry->edges_begin();
    
    for (int i = 0; i < NO_GRASS_STRAW; i++)
    {
        auto pos = patch.get_origo() + vec3(Random::value(0., 0.999 * Terrain::SIZE), 0., Random::value(0., 0.999 * Terrain::SIZE));
        pos.y = patch.get_height_at(pos);
        if(pos.y > 0.15)
        {
            auto straw = vec3(Random::value(-0.1, 0.1), Random::value(0.05, 0.15), Random::value(-0.1, 0.1));
            if(edge == geometry->edges_end())
            {
                edge = geometry->create_edge(geometry->create_vertex(), geometry->create_vertex());
            }
            
            geometry->position()->at(edge->v1()) = pos;
            geometry->position()->at(edge->v2()) = pos + straw;
            
            edge = edge->next();
        }
    }
    
    for (; edge != geometry->edges_end(); edge = edge->next())
    {
        geometry->position()->at(edge->v1()) = vec3(0., 0., 0.);
        geometry->position()->at(edge->v2()) = vec3(0., 0., 0.);
    }
    
    this->invalidate(geometry);
}

void Grass::animate(float time, const glm::vec3& spider_position)
{
    material->spider_position = spider_position;
    material->time = time;
}
