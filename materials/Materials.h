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

class GLGrassMaterial : public gle::GLMaterial
{
    std::shared_ptr<glm::vec3> spider_position;
    std::shared_ptr<glm::vec3> wind;
    glm::vec3 color;
public:
    
    GLGrassMaterial(const std::shared_ptr<glm::vec3> _spider_position,const std::shared_ptr<glm::vec3> _wind, const glm::vec3& _color) : spider_position(_spider_position), wind(_wind), color(_color)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/color_material.frag", "shaders/grass.geom");
    }
    
    bool should_draw(gle::DrawPassMode draw_pass)
    {
        return draw_pass == gle::DEFERRED;
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
        gle::GLUniform::use(shader, "wind", *wind);
        
        gle::GLUniform::use(shader, "materialColor", color);
    }
};

class GLSpiderLegsMaterial : public gle::GLMaterial
{
    glm::vec3 color;
public:
    
    GLSpiderLegsMaterial(const glm::vec3& _color)
        : color(_color)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/color_material.frag", "shaders/spider_legs.geom");
    }
    
    bool should_draw(gle::DrawPassMode draw_pass)
    {
        return draw_pass == gle::DEFERRED;
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

class GLFogMaterial : public gle::GLMaterial
{
    const std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec3>> normals;
    std::shared_ptr<float> time;
    float radius;
public:
    GLFogMaterial(const std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec3>> _normals, const std::shared_ptr<float> _time, float _radius)
        : normals(_normals), time(_time), radius(_radius)
    {
        shader = gle::GLShader::create_or_get("shaders/fog.vert",  "shaders/fog.frag", "../GLEngine/shaders/particle.geom");
    }
    
    bool should_draw(gle::DrawPassMode draw_pass)
    {
        return draw_pass == gle::FORWARD;
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vec2_vertex_attributes,
                                   std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec3>>>& vec3_vertex_attributes)
    {
        vec3_vertex_attributes.push_back(shader->create_attribute("position", geometry->position()));
        vec3_vertex_attributes.push_back(shader->create_attribute("normal", normals));
    }
    
    void pre_draw(const glm::vec3& camera_position, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(false);
        gle::GLState::cull_back_faces(true);
        
        auto modelView = view * model;
        
        gle::GLUniform::use(shader, "eyePosition", camera_position);
        gle::GLUniform::use(shader, "MVMatrix", modelView);
        gle::GLUniform::use(shader, "PMatrix", projection);
        
        gle::GLUniform::use(shader, "radius", radius);
        gle::GLUniform::use(shader, "time", *time);
    }
};

class WaterMaterial : public gle::GLMaterial
{
    std::shared_ptr<float> time;
    std::shared_ptr<gle::GLTexture3D> texture;
public:
    WaterMaterial(const std::shared_ptr<float> _time, std::shared_ptr<gle::GLTexture3D> _texture)
        : texture(_texture) , time(_time)
    {
        shader = gle::GLShader::create_or_get("shaders/water.vert",  "shaders/water.frag");
    }
    
    bool should_draw(gle::DrawPassMode draw_pass)
    {
        return draw_pass == gle::FORWARD;
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vec2_vertex_attributes,
                           std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec3>>>& vec3_vertex_attributes)
    {
        vec3_vertex_attributes.push_back(shader->create_attribute("position", geometry->position()));
        vec3_vertex_attributes.push_back(shader->create_attribute("normal", geometry->normal()));
    }
    
    void pre_draw(const glm::vec3& camera_position, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(false);
        gle::GLState::cull_back_faces(true);
        
        texture->use(0);
        gle::GLUniform::use(shader, "texture0", 0);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(model));
        gle::GLUniform::use(shader, "MVPMatrix", projection * view * model);
        
        gle::GLUniform::use(shader, "eyePosition", camera_position);
//        gle::GLUniform::use(shader, "time", *time);
    }
};

class TerrainMaterial : public gle::GLMaterial
{
    std::shared_ptr<gle::GLTexture> ground_texture;
    std::shared_ptr<gle::GLTexture> lake_texture;
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates;
public:
    
    TerrainMaterial(std::shared_ptr<gle::GLTexture> _ground_texture, std::shared_ptr<gle::GLTexture> _lake_texture, std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> _uv_coordinates)
        : ground_texture(_ground_texture), lake_texture(_lake_texture), uv_coordinates(_uv_coordinates)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/texture.vert",  "shaders/terrain.frag");
    }
    
    bool should_draw(gle::DrawPassMode draw_pass)
    {
        return draw_pass == gle::DEFERRED;
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
        gle::GLState::depth_write(true);
        gle::GLState::cull_back_faces(true);
        
        ground_texture->use(0);
        lake_texture->use(1);
        gle::GLUniform::use(shader, "groundTexture", 0);
        gle::GLUniform::use(shader, "lakeTexture", 1);
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "MVPMatrix", projection * view * model);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(model));
    }
};
