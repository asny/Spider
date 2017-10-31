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
public:
    glm::vec3 color = glm::vec3(0.3f,0.7f,0.f);
    float time = 0.f;
    glm::vec3 spider_position = glm::vec3(0.f,0.f,0.f);
    
    GrassMaterial() : GLMaterial(gle::DEFERRED, "../GLEngine/shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom")
    {
        
    }
    
    void pre_draw(const gle::DrawPassInput& input, const glm::mat4& model)
    {
        auto shader = get_shader();
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(true);
        gle::GLState::cull_back_faces(false);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "VPMatrix", input.projection * input.view);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(model));
        
        gle::GLUniform::use(shader, "spiderPosition", spider_position);
        gle::GLUniform::use(shader, "wind", glm::vec3(0.5 * sin(time) + 0.5, 0., 0.5 * cos(time + 0.5) + 0.5));
        
        gle::GLUniform::use(shader, "materialColor", color);
    }
};

class GLSpiderLegsMaterial : public gle::GLMaterial
{
    glm::vec3 color;
public:
    
    GLSpiderLegsMaterial(const glm::vec3& _color)
        : GLMaterial(gle::DEFERRED, "../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/color_material.frag", "shaders/spider_legs.geom"), color(_color)
    {
        
    }
    
    void pre_draw(const gle::DrawPassInput& input, const glm::mat4& model)
    {
        auto shader = get_shader();
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(true);
        gle::GLState::cull_back_faces(true);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "VPMatrix", input.projection * input.view);
        
        gle::GLUniform::use(shader, "materialColor", color);
    }
};

class WaterMaterial : public gle::GLMaterial
{
    std::shared_ptr<gle::GLTexture> environment_texture, water_foam;
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates;
    
    const float ring_effect_time = 4.;
    std::vector<glm::vec4> ring_effects;
public:
    float time = 0.f;
    glm::vec3 wind_direction = glm::vec3(1., 0., 0.);
    
    float wavelength = 1.0f;
    float speed = 0.5f;
    float wind_variation = 0.1f;
    
    WaterMaterial(std::shared_ptr<gle::GLTexture3D> _environment_texture,
                  std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> _uv_coordinates)
        : GLMaterial(gle::FORWARD, "shaders/water.vert",  "shaders/water.frag"), environment_texture(_environment_texture), uv_coordinates(_uv_coordinates)
    {
        water_foam = std::make_shared<gle::GLTexture2D>("resources/water_foam.png");
        for(int i = 0; i < 32; i++)
        {
            ring_effects.push_back(glm::vec4(0., 0., 0., -1.));
        }
    }
    
    void affect(const glm::vec3& position)
    {
        for(glm::vec4& ring_effect : ring_effects)
        {
            if(ring_effect.w < 0.)
            {
                ring_effect = glm::vec4(position.x, 0., position.z, time);
                return;
            }
        }
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vertex_attributes)
    {
        auto shader = get_shader();
        GLMaterial::create_attributes(geometry, vertex_attributes);
        vertex_attributes.push_back(gle::GLVertexAttribute<glm::vec2>::use(*shader,"uv_coordinates", uv_coordinates));
    }
    
    void pre_draw(const gle::DrawPassInput& input, const glm::mat4& model)
    {
        auto shader = get_shader();
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(false);
        gle::GLState::cull_back_faces(false);
        
        environment_texture->use(0);
        gle::GLUniform::use(shader, "environmentMap", 0);
        water_foam->use(1);
        gle::GLUniform::use(shader, "maskSampler", 1);
        
        input.position_texture->use(2);
        gle::GLUniform::use(shader, "positionMap", 2);
        
        input.shaded_color_texture->use(3);
        gle::GLUniform::use(shader, "colorMap", 3);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "VPMatrix", input.projection * input.view);
        
        gle::GLUniform::use(shader, "eyePosition", input.camera_position);
        gle::GLUniform::use(shader, "screenSize", input.screen_size);
        gle::GLUniform::use(shader, "time", time);
        
        for(glm::vec4& ring_effect : ring_effects)
        {
            if(ring_effect.w > 0. && time > ring_effect.w + ring_effect_time)
            {
                ring_effect = glm::vec4(0., 0., 0., -1.);
            }
        }
        
        gle::GLUniform::use(shader, "ringEffectTime", ring_effect_time);
        gle::GLUniform::use(shader, "noEffects", static_cast<int>(ring_effects.size()));
        gle::GLUniform::use(shader, "ringCenterAndTime", ring_effects[0], static_cast<int>(ring_effects.size()));
        
        gle::GLUniform::use(shader, "median_wavelength", wavelength);
        gle::GLUniform::use(shader, "speed", speed);
        gle::GLUniform::use(shader, "wind_variation", wind_variation);
        gle::GLUniform::use(shader, "wind_direction", glm::vec2(wind_direction.x, wind_direction.z));
        
    }
};

class TerrainMaterial : public gle::GLMaterial
{
    std::shared_ptr<gle::GLTexture> ground_texture, lake_texture, noise_texture;
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates;
public:
    
    TerrainMaterial(std::shared_ptr<gle::GLTexture> _ground_texture, std::shared_ptr<gle::GLTexture> _lake_texture, std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> _uv_coordinates)
        : GLMaterial(gle::DEFERRED, "../GLEngine/shaders/texture.vert",  "shaders/terrain.frag"), ground_texture(_ground_texture), lake_texture(_lake_texture), uv_coordinates(_uv_coordinates)
    {
        create_noise_texture();
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vertex_attributes)
    {
        auto shader = get_shader();
        GLMaterial::create_attributes(geometry, vertex_attributes);
        vertex_attributes.push_back(gle::GLVertexAttribute<glm::vec2>::use(*shader, "uv_coordinates", uv_coordinates));
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec3>>>& vertex_attributes)
    {
        auto shader = get_shader();
        GLMaterial::create_attributes(geometry, vertex_attributes);
        vertex_attributes.push_back(gle::GLVertexAttribute<glm::vec3>::use(*shader, "normal", geometry->normal()));
    }
    
    void pre_draw(const gle::DrawPassInput& input, const glm::mat4& model)
    {
        auto shader = get_shader();
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
        gle::GLUniform::use(shader, "MVPMatrix", input.projection * input.view * model);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(model));
    }
    
    void create_noise_texture()
    {
        const int noise_size = 128;
        auto noise = std::vector<float>(noise_size * noise_size);
        for (int i = 0; i < noise_size * noise_size; ++i)
        {
            noise[i] = gle::random(0., 1.);
        }
        noise_texture = std::unique_ptr<gle::GLTexture2D>(new gle::GLTexture2D(&noise[0], noise_size, noise_size, GL_RED));
    }
};
