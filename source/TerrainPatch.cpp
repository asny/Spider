//
//  Created by Asger Nyman Christiansen on 06/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#include "TerrainPatch.hpp"
#include "glm.hpp"
#include "Random.h"

using namespace std;
using namespace glm;
using namespace gle;

double average(std::vector<double> heights)
{
    if(heights.size() == 0)
    {
        return 0.;
    }
    double sum = 0.;
    for (auto height : heights)
    {
        sum += height;
    }
    return sum / static_cast<double>(heights.size());
}

TerrainPatch::TerrainPatch()
{
    heightmap = vector<vector<double>>(VERTICES_PER_SIDE + 1);
    for ( auto r = 0; r <= VERTICES_PER_SIDE; r++ )
    {
        heightmap[r] = vector<double>(VERTICES_PER_SIDE + 1);
    }
}

void TerrainPatch::update(const vec3& _origo)
{
    origo = _origo;
    
    set_height(SIZE, 0, 0, {});
    set_height(SIZE, 0, VERTICES_PER_SIDE, {});
    set_height(SIZE, VERTICES_PER_SIDE, 0, {});
    set_height(SIZE, VERTICES_PER_SIDE, VERTICES_PER_SIDE, {});
    subdivide(0, 0, VERTICES_PER_SIDE);
}

void TerrainPatch::set_height(double scale, int r, int c, std::vector<double> neighbour_heights)
{
    heightmap[r][c] = average(neighbour_heights) +
    0.15 * scale * static_cast<double>(raw_noise_2d(origo.x + r * VERTEX_DISTANCE, origo.z + c * VERTEX_DISTANCE));
}

void TerrainPatch::subdivide(int origo_r, int origo_c, int size)
{
    int half_size = size/2;
    if(half_size >= 1)
    {
        double scale = size * VERTEX_DISTANCE;
        
        set_height(scale, origo_r + half_size, origo_c, {heightmap[origo_r][origo_c], heightmap[origo_r + size][origo_c]});
        set_height(scale, origo_r, origo_c + half_size, {heightmap[origo_r][origo_c], heightmap[origo_r][origo_c + size]});
        set_height(scale, origo_r + half_size, origo_c + size, {heightmap[origo_r + size][origo_c + size], heightmap[origo_r][origo_c + size]});
        set_height(scale, origo_r + size, origo_c + half_size, {heightmap[origo_r + size][origo_c + size], heightmap[origo_r + size][origo_c]});
        set_height(scale, origo_r + half_size, origo_c + half_size, {heightmap[origo_r + half_size][origo_c], heightmap[origo_r][origo_c + half_size],
            heightmap[origo_r + half_size][origo_c + size], heightmap[origo_r + size][origo_c + half_size]});
        
        subdivide(origo_r, origo_c, half_size);
        subdivide(origo_r + half_size, origo_c, half_size);
        subdivide(origo_r, origo_c + half_size, half_size);
        subdivide(origo_r + half_size, origo_c + half_size, half_size);
    }
}

double TerrainPatch::get_height_at(const vec3& position) const
{
    vec3 vec = position - origo;
    
    double r = floor(vec.x * VERTICES_PER_UNIT);
    double c = floor(vec.z * VERTICES_PER_UNIT);
    
    double tx = vec.x * VERTICES_PER_UNIT - r;
    double tz = vec.z * VERTICES_PER_UNIT - c;
    
    auto height = (1. - tx) * (1. - tz) * heightmap.at(r).at(c);
    height += tx * (1. - tz) * heightmap.at(r+1).at(c);
    height += (1. - tx) * tz * heightmap.at(r).at(c+1);
    height += tx * tz * heightmap.at(r+1).at(c+1);
    return height;
}

glm::vec3 TerrainPatch::get_origo()
{
    return origo;
}
