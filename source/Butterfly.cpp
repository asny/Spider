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

using namespace std;
using namespace gle;
using namespace glm;

Butterfly::Butterfly(GLNode& scene)
{
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates1 = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec2>>();
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates2 = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec2>>();
    
    auto geometry1 = MeshCreator::create_quad();
    auto geometry2 = MeshCreator::create_quad();
    
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
    
    auto global_transformation = std::make_shared<GLTranslationNode>(glm::vec3(0., 2., 0.));
    scene.add_child(global_transformation);
    
    auto transformation1 = std::make_shared<GLTransformationNode>(glm::translate(glm::vec3(-1., 0., 0.)));
    global_transformation->add_child(transformation1);
    
    auto rotation1 = std::make_shared<GLRotationNode>(glm::vec3(0., -1., 0.), butterfly_angle);
    transformation1->add_child(rotation1);
    rotation1->add_leaf(geometry1, material1);
    
    auto transformation2 = std::make_shared<GLTransformationNode>(glm::translate(glm::vec3(1., 0., 0.)));
    global_transformation->add_child(transformation2);
    
    auto rotation2 = std::make_shared<GLRotationNode>(glm::vec3(0., 1., 0.), butterfly_angle);
    transformation2->add_child(rotation2);
    rotation2->add_leaf(geometry2, material2);
}

void Butterfly::update(double time)
{
    *butterfly_angle = sin(10.f * time);
}


