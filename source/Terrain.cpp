//
//  Created by Asger Nyman Christiansen on 05/10/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "glm.hpp"
#include "Terrain.hpp"
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

Terrain::TerrainPatch::TerrainPatch()
{
    heightmap = vector<vector<double>>(VERTICES_PER_SIDE + 1);
    for ( auto r = 0; r <= VERTICES_PER_SIDE; r++ )
    {
        heightmap[r] = vector<double>(VERTICES_PER_SIDE + 1);
    }
}

void Terrain::TerrainPatch::update(const vec3& _origo)
{
    origo = _origo;
    
    set_height(SIZE, 0, 0, {});
    set_height(SIZE, 0, VERTICES_PER_SIDE, {});
    set_height(SIZE, VERTICES_PER_SIDE, 0, {});
    set_height(SIZE, VERTICES_PER_SIDE, VERTICES_PER_SIDE, {});
    subdivide(0, 0, VERTICES_PER_SIDE);
    
    grass_map.clear();
    for (int i = 0; i < NO_GRASS_STRAW; i++)
    {
        auto pos = origo + vec3(Random::value(0., 0.999 * SIZE), 0., Random::value(0., 0.999 * SIZE));
        pos.y = get_height_at(pos);
        if(pos.y > 0.1)
        {
            auto straw = vec3(Random::value(-0.2, 0.2), Random::value(0.1, 0.3), Random::value(-0.2, 0.2));
            grass_map.push_back( { pos, straw } );
        }
    }
}

void Terrain::TerrainPatch::set_height(double scale, int r, int c, std::vector<double> neighbour_heights)
{
    heightmap[r][c] = average(neighbour_heights) +
        0.15 * scale * static_cast<double>(raw_noise_2d(origo.x + r * VERTEX_DISTANCE, origo.z + c * VERTEX_DISTANCE));
}

void Terrain::TerrainPatch::subdivide(int origo_r, int origo_c, int size)
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

double Terrain::TerrainPatch::get_height_at(const vec3& position) const
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

glm::vec3 Terrain::TerrainPatch::get_origo()
{
    return origo;
}

Terrain::Terrain(GLScene& scene, const glm::vec3& _position)
{
    // Initialize ground geometry
    auto ground_uv_coordinates = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec2>>();
    for (int r = 0; r < VERTICES_PER_SIDE; r++)
    {
        for (int c = 0; c < VERTICES_PER_SIDE; c++)
        {
            auto vertex = ground_geometry->create_vertex();
            ground_mapping[pair<int, int>(r,c)] = vertex;
            if(r > 0 && c > 0)
            {
                ground_geometry->create_face(ground_mapping.at(pair<int, int>(r,c-1)), ground_mapping.at(pair<int, int>(r-1,c-1)), vertex);
                ground_geometry->create_face(ground_mapping.at(pair<int, int>(r-1,c)), vertex, ground_mapping.at(pair<int, int>(r-1,c-1)));
            }
            ground_uv_coordinates->at(vertex) = vec2(PATCH_SIDE_LENGTH * r * TerrainPatch::VERTEX_DISTANCE/SIZE, PATCH_SIDE_LENGTH * c * TerrainPatch::VERTEX_DISTANCE/SIZE);
        }
    }
    
    // Initialize water geometry
    mesh::VertexID* v1 = water_geometry->create_vertex();
    mesh::VertexID* v2 = water_geometry->create_vertex();
    mesh::VertexID* v3 = water_geometry->create_vertex();
    mesh::VertexID* v4 = water_geometry->create_vertex();
    auto water_uv_coordinates = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec2>>();
    water_uv_coordinates->at(v1) = vec2(0., 0.);
    water_uv_coordinates->at(v2) = vec2(1., 0.);
    water_uv_coordinates->at(v3) = vec2(1., 1.);
    water_uv_coordinates->at(v4) = vec2(0., 1.);
    water_geometry->create_face(v2, v1, v3);
    water_geometry->create_face(v4, v3, v1);
    
    update(_position);
    
    // Load textures
    auto ground_texture = make_shared<GLTexture2D>("resources/grass.jpg");
    auto lake_texture = make_shared<GLTexture2D>("resources/bottom.png");
    auto noise_texture = make_shared<GLTexture2D>("resources/water_noise.jpg");
    
    const string path = "resources/skybox_evening/";
    auto filenames = {path + "right.jpg", path + "left.jpg", path + "top.jpg", path + "top.jpg", path + "front.jpg", path + "back.jpg"};
    auto skybox_texture = make_shared<GLTexture3D>(filenames);
    
    // Create skybox
    auto skybox_material = make_shared<GLSkyboxMaterial>(skybox_texture);
    auto skybox_geometry = MeshCreator::create_box(true);
    scene.add_leaf(skybox_geometry, skybox_material);
    
    // Create terrain
    auto terrain_material = make_shared<TerrainMaterial>(time, wind_direction, ground_texture, lake_texture, noise_texture, ground_uv_coordinates);
    scene.add_leaf(ground_geometry, terrain_material);
    
    water_material = make_shared<WaterMaterial>(time, wind_direction, skybox_texture, noise_texture, water_uv_coordinates);
    scene.add_leaf(water_geometry, water_material);
    
    auto grass_material = make_shared<GrassMaterial>(time, wind_direction, position, vec3(0.3f,0.7f,0.f));
    scene.add_leaf(grass_geometry, grass_material);
}

pair<int, int> Terrain::index_at(const vec3& position)
{
    vec2 parameter = vec2(position.x, position.z);
    vec2 index_vector = vec2(floor(parameter.x / TerrainPatch::SIZE), floor(parameter.y / TerrainPatch::SIZE));
    return make_pair(static_cast<int>(index_vector.x), static_cast<int>(index_vector.y));
}

Terrain::TerrainPatch* Terrain::patch_at(std::pair<int, int> index)
{
    for (auto& patch : patches)
    {
        if(index == index_at(patch.get_origo()))
            return &patch;
    }
    return nullptr;
}

void Terrain::update(const glm::vec3& _position)
{
    *time = gle::time();
    *position = _position;
    *wind_direction = vec3(1., 0., 0.);
    
    auto index_at_position = index_at(_position);
    
    std::vector<TerrainPatch*> free_patches;
    for (auto& patch : patches)
    {
        auto index = index_at(patch.get_origo());
        if(abs(index_at_position.first - index.first) > 1 || abs(index_at_position.second - index.second) > 1)
        {
            free_patches.push_back(&patch);
        }
    }
    
    bool hasChanged = false;
    
    for (int i = -PATCH_RADIUS; i <= PATCH_RADIUS; i++)
    {
        for (int j = -PATCH_RADIUS; j <= PATCH_RADIUS; j++)
        {
            auto index = make_pair(index_at_position.first + i, index_at_position.second + j);
            auto patch = patch_at(index);
            if(!patch)
            {
                TerrainPatch* patch;
                if(free_patches.size() == 0)
                {
                    patches.push_back(TerrainPatch());
                    patch = &patches.back();
                }
                else {
                    patch = free_patches.back();
                    free_patches.pop_back();
                }
                vec3 origo = vec3(TerrainPatch::SIZE * static_cast<double>(index.first), 0., TerrainPatch::SIZE * static_cast<double>(index.second));
                patch->update(origo);
                hasChanged = true;
            }
        }
    }
    
    if(!hasChanged)
        return;
    
    auto index = make_pair(index_at_position.first - PATCH_RADIUS, index_at_position.second - PATCH_RADIUS);
    auto patch = patch_at(index);
    auto origo = patch->get_origo();
    
    // Update ground geometry
    for (int r = 0; r < VERTICES_PER_SIDE; r++)
    {
        for (int c = 0; c < VERTICES_PER_SIDE; c++)
        {
            vec3 pos = origo + vec3(r * TerrainPatch::VERTEX_DISTANCE, 0., c * TerrainPatch::VERTEX_DISTANCE);
            pos.y = get_height_at(pos);
            auto ground_vertex = ground_mapping.at(pair<int, int>(r,c));
            ground_geometry->position()->at(ground_vertex) = pos;
        }
    }
    
    // Update ground normals
    ground_geometry->update_normals();
    
    // Update grass geometry
    auto edge = grass_geometry->edges_begin();
    for (auto patch : patches)
    {
        for (auto grass_straw : patch.get_grass_straws())
        {
            if(edge == grass_geometry->edges_end())
            {
                edge = grass_geometry->create_edge(grass_geometry->create_vertex(), grass_geometry->create_vertex());
            }
            
            grass_geometry->position()->at(edge->v1()) = grass_straw.position;
            grass_geometry->position()->at(edge->v2()) = grass_straw.position + grass_straw.straw;
            
            edge = edge->next();
        }
    }
    
    for (; edge != grass_geometry->edges_end(); edge = edge->next())
    {
        grass_geometry->position()->at(edge->v1()) = vec3(0., 0., 0.);
        grass_geometry->position()->at(edge->v2()) = vec3(0., 0., 0.);
    }
    
    // Update water geometry
    auto vertex = water_geometry->vertices_begin();
    water_geometry->position()->at(vertex) = origo;
    vertex = vertex->next();
    water_geometry->position()->at(vertex) = origo + glm::vec3(SIZE, 0., 0.);
    vertex = vertex->next();
    water_geometry->position()->at(vertex) = origo + glm::vec3(SIZE, 0., SIZE);
    vertex = vertex->next();
    water_geometry->position()->at(vertex) = origo + glm::vec3(0., 0., SIZE);
}

double Terrain::get_height_at(const glm::vec3& position)
{
    auto index = index_at(position);
    TerrainPatch* patch = patch_at(index);
    return patch->get_height_at(position);
}
