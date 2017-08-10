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
    water_material = make_shared<WaterMaterial>(skybox_texture, ground_uv_coordinates);
    scene->add_leaf(ground_geometry, water_material);
}

pair<int, int> Environment::index_at(const vec3& position)
{
    vec2 parameter = vec2(position.x, position.z);
    vec2 index_vector = vec2(floor(parameter.x / Terrain::SIZE), floor(parameter.y / Terrain::SIZE));
    return make_pair(static_cast<int>(index_vector.x), static_cast<int>(index_vector.y));
}

std::shared_ptr<TerrainPatch> Environment::patch_at(std::pair<int, int> index)
{
    for (auto& patch : patches)
    {
        if(index == index_at(patch->get_terrain().get_origo()))
            return patch;
    }
    return nullptr;
}

void Environment::update_patches(const pair<int, int>& index_at_position)
{
    std::vector<std::shared_ptr<TerrainPatch>> free_patches;
    for (auto& patch : patches)
    {
        auto index = index_at(patch->get_terrain().get_origo());
        if(abs(index_at_position.first - index.first) > PATCH_RADIUS || abs(index_at_position.second - index.second) > PATCH_RADIUS)
        {
            free_patches.push_back(patch);
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
                std::shared_ptr<TerrainPatch> patch;
                if(free_patches.size() == 0)
                {
                    patches.push_back(std::make_shared<TerrainPatch>());
                    patch = patches.back();
                    scene->add_child(patch);
                }
                else {
                    patch = free_patches.back();
                    free_patches.pop_back();
                }
                vec3 origo = vec3(Terrain::SIZE * static_cast<double>(index.first), 0., Terrain::SIZE * static_cast<double>(index.second));
                patch->initialize(origo);
            }
        }
    }
    current_center = index_at_position;
    
    // Update ground geometry
    vec3 origo = vec3(Terrain::SIZE * static_cast<double>(index_at_position.first - PATCH_RADIUS), 0.,
                      Terrain::SIZE * static_cast<double>(index_at_position.second - PATCH_RADIUS));
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
    auto time = gle::time();
    auto wind_direction = vec3(1., 0., 0.);
    
    for (auto& patch : patches)
    {
        patch->animate(time, _position, wind_direction);
    }
    water_material->time = time;
    water_material->wind_direction = wind_direction;
    
    auto index_at_position = index_at(_position);
    if(patches.size() == 0)
    {
        update_patches(index_at_position);
    }
    else if(!is_generating && current_center != index_at_position)
    {
        is_generating = true;
        std::function<void(const pair<int, int>& index_at_position)> f = std::bind(&Environment::update_patches, this, std::placeholders::_1);
        fut = std::async(std::launch::async, f, index_at_position);
    }
}

bool Environment::is_inside(const glm::vec3& position)
{
    auto index = index_at(position);
    return abs(current_center.first - index.first) <= PATCH_RADIUS && abs(current_center.second - index.second) <= PATCH_RADIUS;
}

double Environment::get_height_at(const glm::vec3& position)
{
    auto index = index_at(position);
    auto patch = patch_at(index);
    return patch->get_terrain().get_height_at(position);
}
