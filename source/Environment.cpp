//
//  Created by Asger Nyman Christiansen on 06/08/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#include "Environment.hpp"

using namespace gle;
using namespace std;
using namespace glm;

Environment::Environment(GLScene* scene) : scene(scene)
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
            ground_uv_coordinates->at(vertex) = vec2(PATCH_SIDE_LENGTH * r * Terrain::VERTEX_DISTANCE/SIZE, PATCH_SIDE_LENGTH * c * Terrain::VERTEX_DISTANCE/SIZE);
        }
    }
    
    // Load textures
    auto ground_texture = make_shared<GLTexture2D>("resources/grass.jpg");
    auto lake_texture = make_shared<GLTexture2D>("resources/bottom.png");
    
    const string path = "resources/skybox_evening/";
    auto filenames = {path + "right.jpg", path + "left.jpg", path + "top.jpg", path + "top.jpg", path + "front.jpg", path + "back.jpg"};
    auto skybox_texture = make_shared<GLTexture3D>(filenames);
    
    // Create skybox
    auto skybox_material = make_shared<GLSkyboxMaterial>(skybox_texture);
    auto skybox_geometry = MeshCreator::create_box(true);
    scene->add_leaf(skybox_geometry, skybox_material);
    
    // Create terrain
    auto terrain_material = make_shared<TerrainMaterial>(ground_texture, lake_texture, ground_uv_coordinates);
    scene->add_leaf(ground_geometry, terrain_material);
    
    // Create water
    water_material = make_shared<WaterMaterial>(time, wind_direction, skybox_texture, ground_uv_coordinates);
    scene->add_leaf(ground_geometry, water_material);
}

pair<int, int> Environment::index_at(const vec3& position)
{
    vec2 parameter = vec2(position.x, position.z);
    vec2 index_vector = vec2(floor(parameter.x / Terrain::SIZE), floor(parameter.y / Terrain::SIZE));
    return make_pair(static_cast<int>(index_vector.x), static_cast<int>(index_vector.y));
}

TerrainPatch* Environment::patch_at(std::pair<int, int> index)
{
    for (auto& patch : patches)
    {
        if(index == index_at(patch.get_terrain().get_origo()))
            return &patch;
    }
    return nullptr;
}

bool Environment::should_generate_patches(const pair<int, int>& index_at_position)
{
    for (auto& patch : patches)
    {
        auto index = index_at(patch.get_terrain().get_origo());
        if(abs(index_at_position.first - index.first) > 1 || abs(index_at_position.second - index.second) > 1)
        {
            return true;
        }
    }
    return false;
}

void Environment::update_patches(const pair<int, int>& index_at_position)
{
    std::vector<TerrainPatch*> free_patches;
    for (auto& patch : patches)
    {
        auto index = index_at(patch.get_terrain().get_origo());
        if(abs(index_at_position.first - index.first) > 1 || abs(index_at_position.second - index.second) > 1)
        {
            free_patches.push_back(&patch);
        }
    }
    
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
                    patches.push_back(TerrainPatch(*scene));
                    patch = &patches.back();
                }
                else {
                    patch = free_patches.back();
                    free_patches.pop_back();
                }
                vec3 origo = vec3(Terrain::SIZE * static_cast<double>(index.first), 0., Terrain::SIZE * static_cast<double>(index.second));
                patch->get_terrain().update(origo);
                patch->update_grass();
            }
        }
    }
    
    auto index = make_pair(index_at_position.first - PATCH_RADIUS, index_at_position.second - PATCH_RADIUS);
    auto patch = patch_at(index);
    auto origo = patch->get_terrain().get_origo();
    
    // Update ground geometry
    for (int r = 0; r < VERTICES_PER_SIDE; r++)
    {
        for (int c = 0; c < VERTICES_PER_SIDE; c++)
        {
            vec3 pos = origo + vec3(r * Terrain::VERTEX_DISTANCE, 0., c * Terrain::VERTEX_DISTANCE);
            pos.y = get_height_at(pos);
            auto ground_vertex = ground_mapping.at(pair<int, int>(r,c));
            ground_geometry->position()->at(ground_vertex) = pos;
        }
    }
    
    // Update ground normals
    ground_geometry->update_normals();
    
    scene->invalidate(ground_geometry);
    
    is_generating = false;
}

void Environment::update(const glm::vec3& _position)
{
    *time = gle::time();
    *position = _position;
    *wind_direction = vec3(1., 0., 0.);
    
    for (auto& patch : patches)
    {
        patch.update(*time, *position);
    }
    
    auto index_at_position = index_at(_position);
    if(patches.size() == 0)
    {
        update_patches(index_at_position);
    }
    else if(!is_generating && should_generate_patches(index_at_position))
    {
        is_generating = true;
        std::function<void(const pair<int, int>& index_at_position)> f = std::bind(&Environment::update_patches, this, std::placeholders::_1);
        fut = std::async(std::launch::async, f, index_at_position);
    }
}

bool Environment::is_inside(const glm::vec3& position)
{
    auto index = index_at(position);
    return patch_at(index);
}

double Environment::get_height_at(const glm::vec3& position)
{
    auto index = index_at(position);
    TerrainPatch* patch = patch_at(index);
    return patch->get_terrain().get_height_at(position);
}
