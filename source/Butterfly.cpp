//
//  Butterfly.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/02/2017.
//  Copyright © 2017 Asger Nyman Christiansen. All rights reserved.
//

#include "Butterfly.hpp"
#include "MeshCreator.h"
#include "Materials.h"
#include "gtc/matrix_transform.hpp"
#include "gtx/rotate_vector.hpp"
#include "Random.h"

using namespace std;
using namespace gle;
using namespace glm;

Butterfly::Butterfly(const glm::vec3& spider_position)
{
    static std::shared_ptr<gle::GLNode> wing_node1 = nullptr;
    static std::shared_ptr<gle::GLNode> wing_node2 = nullptr;
    
    if(!wing_node1)
    {
        std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates1 = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec2>>();
        std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates2 = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec2>>();
        
        auto geometry1 = MeshCreator::create_quad();
        auto geometry2 = MeshCreator::create_quad();
        geometry1->transform(glm::scale(glm::vec3(0.2, 0.2, 0.2)));
        geometry2->transform(glm::scale(glm::vec3(0.2, 0.2, 0.2)));
        
        auto vertexId = geometry1->vertices_begin();
        uv_coordinates1->at(vertexId) = glm::vec2(0., 0.);
        vertexId = vertexId->next();
        uv_coordinates1->at(vertexId) = glm::vec2(0., 1.);
        vertexId = vertexId->next();
        uv_coordinates1->at(vertexId) = glm::vec2(0.5, 1.);
        vertexId = vertexId->next();
        uv_coordinates1->at(vertexId) = glm::vec2(0.5, 0.);
        
        vertexId = geometry2->vertices_begin();
        uv_coordinates2->at(vertexId) = glm::vec2(0.5, 0.);
        vertexId = vertexId->next();
        uv_coordinates2->at(vertexId) = glm::vec2(0.5, 1.);
        vertexId = vertexId->next();
        uv_coordinates2->at(vertexId) = glm::vec2(1., 1.);
        vertexId = vertexId->next();
        uv_coordinates2->at(vertexId) = glm::vec2(1., 0.);
        
        auto texture = make_shared<GLTexture2D>("resources/butterfly2.png");
        
        auto material1 = make_shared<GLTextureMaterial>(texture, uv_coordinates1);
        auto material2 = make_shared<GLTextureMaterial>(texture, uv_coordinates2);
        material1->cull_back_faces = false;
        material2->cull_back_faces = false;
        
        wing_node1 = std::make_shared<gle::GLNode>();
        wing_node2 = std::make_shared<gle::GLNode>();
        
        wing_node1->add_leaf(geometry1, material1);
        wing_node2->add_leaf(geometry2, material2);
    }
    
    auto global_transformation = std::make_shared<GLTransformationNode>(local2world);
    this->add_child(global_transformation);
    
    // First wing
    auto rotation1 = std::make_shared<GLRotationNode>(glm::vec3(0., -1., 0.), wing_angle);
    global_transformation->add_child(rotation1);
    
    auto transformation1 = std::make_shared<GLTransformationNode>(glm::translate(glm::vec3(-0.2, 0., 0.)));
    rotation1->add_child(transformation1);
    
    transformation1->add_child(wing_node1);
    
    // Second wing
    auto rotation2 = std::make_shared<GLRotationNode>(glm::vec3(0., 1., 0.), wing_angle);
    global_transformation->add_child(rotation2);
    
    auto transformation2 = std::make_shared<GLTransformationNode>(glm::translate(glm::vec3(0.2, 0., 0.)));
    rotation2->add_child(transformation2);
    
    transformation2->add_child(wing_node2);
    
    // Initialise configuration
    translation = glm::translate(spider_position + Random::value(4.f, 5.f) * Random::hemisphere_direction(vec3(0., 1., 0.)));
    rotation = orientation(Random::direction(), vec3(0., 1., 0.));
    start_wing_angle = Random::value(0., pi<double>());
}

float repel_function(float distance)
{
    return std::max(1.f - distance*distance, 0.f);
}

void Butterfly::update(Environment& environment, const Spider& spider)
{
    float time = gle::time();
    float elapsed_time = time - last_time;
    last_time = time;
    
    vec3 current_position = vec3(translation[3]);
    float terrain_height = environment.get_height_at(current_position);
    vec3 spider_position = spider.get_position(environment);
    
    // Wing
    *wing_angle = sin(start_wing_angle + 30.*time);
    
    // Direction
    vec3 current_direction = mat3(rotation) * vec3(0., 1., 0.);
    vec3 new_direction = current_direction + 0.1f * Random::direction();
    
    // Repel ground/sky
    float dist_to_ground = std::max(abs(current_position.y - terrain_height), 0.2f);
    float dist_to_sky = abs(5.f - current_position.y);
    new_direction += (repel_function(dist_to_ground) - repel_function(dist_to_sky)) * vec3(0.f, 1.f, 0.f);
    
    // Repel spider
    float dist_to_spider = glm::distance(current_position, spider_position);
    new_direction += repel_function(dist_to_spider) * normalize(current_position - spider_position);
    
    // Rotate
    rotation = orientation(normalize(new_direction), vec3(0., 1., 0.));
    
    // Move
    const float speed = 0.8f;
    translation *= translate(mat3(rotation) * vec3(0., speed * elapsed_time, 0.));
    
    // Update local to world
    *local2world = translation * rotation;
    
}


