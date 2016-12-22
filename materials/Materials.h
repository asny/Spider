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

class GLGrassMaterial : public oogl::GLMaterial
{
public:
    
    GLGrassMaterial(const std::shared_ptr<glm::vec3> spiderPosition, const std::shared_ptr<glm::vec3> wind, const glm::vec3& _ambient, const glm::vec3& _diffuse, double _opacity)
    {
        shader = oogl::GLShader::create_or_get("../GLEngine/shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom");
        
        use_uniform("VMatrix", view);
        use_uniform("MVMatrix", modelView);
        use_uniform("NMatrix", inverseModelView);
        use_uniform("PMatrix", projection);
        
        use_uniform("spiderPosition", spiderPosition);
        use_uniform("wind", wind);
        use_uniform("ambientMat", _ambient);
        use_uniform("diffuseMat", _diffuse);
        use_uniform("opacity", _opacity);
        
        cull_back_faces = false;
        test_depth = _opacity >= 0.999;
    }
};

class GLSpiderLegsMaterial : public oogl::GLMaterial
{
public:
    
    GLSpiderLegsMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, double _opacity)
    {
        shader = oogl::GLShader::create_or_get("../GLEngine/shaders/pre_geom.vert",  "../GLEngine/shaders/phong.frag", "shaders/spider_legs.geom");
        
        use_uniform("VMatrix", view);
        use_uniform("MVMatrix", modelView);
        use_uniform("NMatrix", inverseModelView);
        use_uniform("PMatrix", projection);
        use_uniform("ambientMat", _ambient);
        use_uniform("diffuseMat", _diffuse);
        use_uniform("specMat", _specular);
        use_uniform("opacity", _opacity);
        
        test_depth = _opacity >= 0.999;
    }
};

class GLFogMaterial : public oogl::GLMaterial
{
    const std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> normals;
    
public:
    GLFogMaterial(const std::shared_ptr<geogo::Attribute<geogo::VertexID, glm::vec3>> _normals, const std::shared_ptr<float> time, float radius) : normals(_normals)
    {
        shader = oogl::GLShader::create_or_get("shaders/fog.vert",  "shaders/fog.frag", "../GLEngine/shaders/particle.geom");
        
        use_uniform("MVMatrix", modelView);
        use_uniform("PMatrix", projection);
        use_uniform("eyePosition", eye_position);
        use_uniform("radius", radius);
        use_uniform("time", time);
        
        test_depth = false;
    }
    
    void create_attributes(std::vector<std::shared_ptr<oogl::GLVertexAttribute<glm::vec2>>>& vec2_vertex_attributes,
                           std::vector<std::shared_ptr<oogl::GLVertexAttribute<glm::vec3>>>& vec3_vertex_attributes)
    {
        vec3_vertex_attributes.push_back(create_attribute("normal", normals));
    }
};
