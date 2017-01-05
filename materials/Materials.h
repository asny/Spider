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

class GLGrassMaterial : public gle::GLMaterial
{
    std::shared_ptr<glm::vec3> spider_position;
    std::shared_ptr<glm::vec3> wind;
    
    glm::vec3 ambient, diffuse;
    float opacity;
public:
    
    GLGrassMaterial(const std::shared_ptr<glm::vec3> _spider_position, const std::shared_ptr<glm::vec3> _wind, const glm::vec3& _ambient, const glm::vec3& _diffuse, float _opacity) : spider_position(_spider_position), wind(_wind), ambient(_ambient), diffuse(_diffuse), opacity(_opacity)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom");
    }
    
    bool should_draw(gle::DrawPassMode draw_pass)
    {
        return draw_pass == gle::FORWARD;
    }
    
    void pre_draw(const glm::vec3& camera_position, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    {
        shader->depth_test(opacity >= 0.999);
        shader->cull_back_faces(false);
        
        auto modelView = view * model;
        
        gle::GLUniform::use(shader, "VMatrix", view);
        gle::GLUniform::use(shader, "MVMatrix", modelView);
        gle::GLUniform::use(shader, "PMatrix", projection);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(modelView));
        
        gle::GLUniform::use(shader, "spiderPosition", *spider_position);
        gle::GLUniform::use(shader, "wind", *wind);
        
        gle::GLUniform::use(shader, "ambientMat", ambient);
        gle::GLUniform::use(shader, "diffuseMat", diffuse);
        gle::GLUniform::use(shader, "opacity", opacity);
    }
};

class GLSpiderLegsMaterial : public gle::GLMaterial
{
    glm::vec3 ambient, diffuse, specular;
    float opacity;
public:
    
    GLSpiderLegsMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, float _opacity)
        : ambient(_ambient), diffuse(_diffuse), specular(_specular), opacity(_opacity)
    {
        shader = gle::GLShader::create_or_get("../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/phong.frag", "shaders/spider_legs.geom");
    }
    
    bool should_draw(gle::DrawPassMode draw_pass)
    {
        return draw_pass == gle::FORWARD;
    }
    
    void pre_draw(const glm::vec3& camera_position, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    {
        shader->depth_test(opacity >= 0.999);
        shader->cull_back_faces(true);
        
        auto modelView = view * model;
        
        gle::GLUniform::use(shader, "VMatrix", view);
        gle::GLUniform::use(shader, "MVMatrix", modelView);
        gle::GLUniform::use(shader, "PMatrix", projection);
        gle::GLUniform::use(shader, "NMatrix", inverseTranspose(modelView));
        
        gle::GLUniform::use(shader, "ambientMat", ambient);
        gle::GLUniform::use(shader, "diffuseMat", diffuse);
        gle::GLUniform::use(shader, "specMat", specular);
        gle::GLUniform::use(shader, "opacity", opacity);
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
        shader->depth_test(false);
        shader->cull_back_faces(true);
        
        auto modelView = view * model;
        
        gle::GLUniform::use(shader, "eyePosition", camera_position);
        gle::GLUniform::use(shader, "MVMatrix", modelView);
        gle::GLUniform::use(shader, "PMatrix", projection);
        
        gle::GLUniform::use(shader, "radius", radius);
        gle::GLUniform::use(shader, "time", *time);
    }
};
