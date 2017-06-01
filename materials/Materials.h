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
        : GLMaterial(gle::DEFERRED, "../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/color_material.frag", "shaders/grass.geom"), spider_position(_spider_position), time(_time), wind_direction(_wind_direction), color(_color)
    {
        
    }
    
    void pre_draw(const gle::DrawPassInput& input, const glm::mat4& model)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(true);
        gle::GLState::cull_back_faces(false);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "VPMatrix", input.projection * input.view);
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
        : GLMaterial(gle::DEFERRED, "../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/color_material.frag", "shaders/spider_legs.geom"), color(_color)
    {
        
    }
    
    void pre_draw(const gle::DrawPassInput& input, const glm::mat4& model)
    {
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
    std::shared_ptr<float> time;
    std::shared_ptr<glm::vec3> wind_direction;
    std::shared_ptr<gle::GLTexture> environment_texture, noise_texture;
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates;
public:
    WaterMaterial(const std::shared_ptr<float> _time, const std::shared_ptr<glm::vec3> _wind_direction, std::shared_ptr<gle::GLTexture3D> _environment_texture, std::shared_ptr<gle::GLTexture> _noise_texture, std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> _uv_coordinates)
        : GLMaterial(gle::FORWARD, "shaders/water.vert",  "shaders/water.frag"), environment_texture(_environment_texture), time(_time), wind_direction(_wind_direction), noise_texture(_noise_texture), uv_coordinates(_uv_coordinates)
    {
        
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vertex_attributes)
    {
        GLMaterial::create_attributes(geometry, vertex_attributes);
        vertex_attributes.push_back(shader->create_attribute("uv_coordinates", uv_coordinates));
    }
    
    void pre_draw(const gle::DrawPassInput& input, const glm::mat4& model)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(false);
        gle::GLState::cull_back_faces(true);
        
        environment_texture->use(0);
        gle::GLUniform::use(shader, "environmentMap", 0);
        noise_texture->use(1);
        gle::GLUniform::use(shader, "noiseTexture", 1);
        
        input.source_render_target->bind_color_texture_for_reading(1, 2);
        gle::GLUniform::use(shader, "positionMap", 2);
        
        input.source_render_target->bind_color_texture_for_reading(0, 3);
        gle::GLUniform::use(shader, "colorMap", 3);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "VPMatrix", input.projection * input.view);
        
        gle::GLUniform::use(shader, "eyePosition", input.camera_position);
        gle::GLUniform::use(shader, "time", *time);
        
        auto amplitude = std::vector<float>();
        auto wavelength = std::vector<float>();
        auto speed = std::vector<float>();
        auto direction = std::vector<glm::vec2>();
        
        const int no_waves = 4;
        for(int i = 0; i < no_waves; i++)
        {
            float t = i + 1.f;
            amplitude.push_back(0.1f / t);
            wavelength.push_back(4 * M_PI / t);
            speed.push_back(0.1f + 0.2*i);
            direction.push_back(normalize(glm::vec2(wind_direction->x, wind_direction->z) + 0.1f * glm::vec2(sin(0.01* *time), cos(0.01* *time))));
        }
        
        gle::GLUniform::use(shader, "amplitude", amplitude[0], no_waves);
        gle::GLUniform::use(shader, "wavelength", wavelength[0], no_waves);
        gle::GLUniform::use(shader, "speed", speed[0], no_waves);
        gle::GLUniform::use(shader, "direction", direction[0], no_waves);
        gle::GLUniform::use(shader, "noWaves", no_waves);
        
    }
};

class TerrainMaterial : public gle::GLMaterial
{
    std::shared_ptr<float> time;
    std::shared_ptr<glm::vec3> wind_direction;
    std::shared_ptr<gle::GLTexture> ground_texture, lake_texture, water_noise_texture, noise_texture;
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> uv_coordinates;
public:
    
    TerrainMaterial(const std::shared_ptr<float> _time, const std::shared_ptr<glm::vec3> _wind_direction, std::shared_ptr<gle::GLTexture> _ground_texture, std::shared_ptr<gle::GLTexture> _lake_texture, std::shared_ptr<gle::GLTexture> _water_noise_texture, std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec2>> _uv_coordinates)
        : GLMaterial(gle::DEFERRED, "../GLEngine/shaders/texture.vert",  "shaders/terrain.frag"), time(_time), wind_direction(_wind_direction), ground_texture(_ground_texture), lake_texture(_lake_texture), uv_coordinates(_uv_coordinates), water_noise_texture(_water_noise_texture)
    {
        create_noise_texture();
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec2>>>& vertex_attributes)
    {
        GLMaterial::create_attributes(geometry, vertex_attributes);
        vertex_attributes.push_back(shader->create_attribute("uv_coordinates", uv_coordinates));
    }
    
    void create_attributes(std::shared_ptr<mesh::Mesh> geometry, std::vector<std::shared_ptr<gle::GLVertexAttribute<glm::vec3>>>& vertex_attributes)
    {
        GLMaterial::create_attributes(geometry, vertex_attributes);
        vertex_attributes.push_back(shader->create_attribute("normal", geometry->normal()));
    }
    
    void pre_draw(const gle::DrawPassInput& input, const glm::mat4& model)
    {
        gle::GLState::depth_test(true);
        gle::GLState::depth_write(true);
        gle::GLState::cull_back_faces(true);
        
        ground_texture->use(0);
        lake_texture->use(1);
        gle::GLUniform::use(shader, "groundTexture", 0);
        gle::GLUniform::use(shader, "lakeTexture", 1);
        water_noise_texture->use(2);
        gle::GLUniform::use(shader, "waterNoiseTexture", 2);
        noise_texture->use(3);
        gle::GLUniform::use(shader, "noiseTexture", 3);
        
        gle::GLUniform::use(shader, "MMatrix", model);
        gle::GLUniform::use(shader, "MVPMatrix", input.projection * input.view * model);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(model));
        gle::GLUniform::use(shader, "time", *time);
        gle::GLUniform::use(shader, "windDirection", *wind_direction);
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
