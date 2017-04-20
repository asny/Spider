//
//  Materials.h
//  Spider
//
//  Created by Asger Nyman Christiansen on 22/12/2016.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLMaterial.h"
#include "materials/GLStandardMaterial.h"
#include "materials/GLTextureMaterial.h"
#include "materials/GLSkyboxMaterial.h"
#include "materials/GLColorMaterial.h"

class GrassMaterial : public gle::GLMaterial
{
    std::shared_ptr<glm::vec3> spider_position;
    std::shared_ptr<glm::vec3> wind_direction;
    std::shared_ptr<float> time;
    glm::vec3 color;
public:
    
    GrassMaterial(const std::shared_ptr<float> _time, const std::shared_ptr<glm::vec3> _wind_direction, const std::shared_ptr<glm::vec3> _spider_position, const glm::vec3& _color)
        : GLMaterial(gle::DEFERRED), spider_position(_spider_position), time(_time), wind_direction(_wind_direction), color(_color)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/color_material.frag", "shaders/grass.geom");
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vec2_vertex_attributes,
                           std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec3>>>& vec3_vertex_attributes)
    {
        vec3_vertex_attributes.push_back(shader->create_attribute("position", geometry->position()));
    }
    
    void pre_draw(const glm::vec3& camera_position, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(true);
        gle::GLState::cull_back_faces(false);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "VPMatrix", projection * view);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(model));
        
        gle::GLUniform::use(shader, "spiderPosition", *spider_position);
        gle::GLUniform::use(shader, "wind", glm::vec3(0.5 * sin(*time) + 0.5, 0., 0.5 * cos(*time + 0.5) + 0.5));
        
        gle::GLUniform::use(shader, "materialColor", color);
    }
};

class GLSpiderLegsMaterial : public gle::GLMaterial
{
    glm::vec3 color;
public:
    
    GLSpiderLegsMaterial(const glm::vec3& _color)
        : GLMaterial(gle::DEFERRED), color(_color)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/color_material.frag", "shaders/spider_legs.geom");
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vec2_vertex_attributes,
                           std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec3>>>& vec3_vertex_attributes)
    {
        vec3_vertex_attributes.push_back(shader->create_attribute("position", geometry->position()));
    }
    
    void pre_draw(const glm::vec3& camera_position, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(true);
        gle::GLState::cull_back_faces(true);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "VPMatrix", projection * view);
        
        gle::GLUniform::use(shader, "materialColor", color);
    }
};

class WaterMaterial : public gle::GLMaterial
{
    std::shared_ptr<float> time;
    std::shared_ptr<glm::vec3> wind_direction;
    std::shared_ptr<gle::GLTexture> texture, noise_texture;
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates;
public:
    WaterMaterial(const std::shared_ptr<float> _time, const std::shared_ptr<glm::vec3> _wind_direction, std::shared_ptr<gle::GLTexture3D> _texture, std::shared_ptr<gle::GLTexture> _noise_texture, std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> _uv_coordinates)
        : GLMaterial(gle::FORWARD), texture(_texture), time(_time), wind_direction(_wind_direction), noise_texture(_noise_texture), uv_coordinates(_uv_coordinates)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/texture.vert",  "shaders/water.frag");
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vec2_vertex_attributes,
                           std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec3>>>& vec3_vertex_attributes)
    {
        vec3_vertex_attributes.push_back(shader->create_attribute("position", geometry->position()));
        vec3_vertex_attributes.push_back(shader->create_attribute("normal", geometry->normal()));
        vec2_vertex_attributes.push_back(shader->create_attribute("uv_coordinates", uv_coordinates));
    }
    
    void pre_draw(const glm::vec3& camera_position, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(false);
        gle::GLState::cull_back_faces(true);
        
        texture->use(0);
        gle::GLUniform::use(shader, "texture0", 0);
        noise_texture->use(1);
        gle::GLUniform::use(shader, "noiseTexture", 1);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(model));
        gle::GLUniform::use(shader, "MVPMatrix", projection * view * model);
        
        gle::GLUniform::use(shader, "eyePosition", camera_position);
        gle::GLUniform::use(shader, "time", *time);
        gle::GLUniform::use(shader, "windDirection", *wind_direction);
    }
};

class TerrainMaterial : public gle::GLMaterial
{
    std::shared_ptr<float> time;
    std::shared_ptr<glm::vec3> wind_direction;
    std::shared_ptr<gle::GLTexture> ground_texture, lake_texture, noise_texture;
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates;
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec3>> normals;
public:
    
    TerrainMaterial(const std::shared_ptr<float> _time, const std::shared_ptr<glm::vec3> _wind_direction, std::shared_ptr<gle::GLTexture> _ground_texture, std::shared_ptr<gle::GLTexture> _lake_texture, std::shared_ptr<gle::GLTexture> _noise_texture, std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> _uv_coordinates, std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec3>> _normals)
        : GLMaterial(gle::DEFERRED), time(_time), wind_direction(_wind_direction), ground_texture(_ground_texture), lake_texture(_lake_texture), uv_coordinates(_uv_coordinates), noise_texture(_noise_texture), normals(_normals)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/texture.vert",  "shaders/terrain.frag");
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vec2_vertex_attributes,
                           std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec3>>>& vec3_vertex_attributes)
    {
        vec3_vertex_attributes.push_back(shader->create_attribute("position", geometry->position()));
        vec3_vertex_attributes.push_back(shader->create_attribute("normal", normals));
        vec2_vertex_attributes.push_back(shader->create_attribute("uv_coordinates", uv_coordinates));
    }
    
    void pre_draw(const glm::vec3& camera_position, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(true);
        gle::GLState::cull_back_faces(true);
        
        ground_texture->use(0);
        lake_texture->use(1);
        gle::GLUniform::use(shader, "groundTexture", 0);
        gle::GLUniform::use(shader, "lakeTexture", 1);
        noise_texture->use(2);
        gle::GLUniform::use(shader, "noiseTexture", 2);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "MVPMatrix", projection * view * model);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(model));
        gle::GLUniform::use(shader, "time", *time);
        gle::GLUniform::use(shader, "windDirection", *wind_direction);
    }
};
