//
//  GLMaterial.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 27/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLTexture.hpp"
#include "GLShader.hpp"
#include "GLUniform.hpp"

namespace oogl
{
    class GLMaterial
    {
        static bool currently_cull_back_faces;
        static bool currently_test_depth;
        
    protected:
        bool cull_back_faces = true;
        bool test_depth = true;
        
        std::shared_ptr<GLShader> shader;
        
        GLMaterial()
        {
        }
        
    public:
        
        GLuint get_uniform_location(const std::string& name)
        {
            return shader->get_uniform_location(name);
        }
        
        GLuint get_attribute_location(const std::string& name)
        {
            return shader->get_attribute_location(name);
        }
        
        virtual void pre_draw();
    };
    
    class GLStandardMaterial : public GLMaterial
    {
        std::unique_ptr<GLUniform<glm::vec3>> ambient;
        std::unique_ptr<GLUniform<glm::vec3>> diffuse;
        std::unique_ptr<GLUniform<glm::vec3>> specular;
        std::unique_ptr<GLUniform<float>> opacity;
    public:
        
        GLStandardMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, double _opacity)
        {
            shader = std::make_shared<GLShader>("shaders/phong.vert",  "shaders/phong.frag");
            
            ambient = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("ambientMat"), _ambient));
            diffuse = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("diffuseMat"), _diffuse));
            specular = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("specMat"), _specular));
            opacity = std::unique_ptr<GLUniform<float>>(new GLUniform<float>(get_uniform_location("opacity"), _opacity));
            test_depth = _opacity >= 0.999;
        }
        
        void pre_draw();
    };
    
    class GLFlatMaterial : public GLMaterial
    {
        std::unique_ptr<GLUniform<glm::vec3>> ambient;
        std::unique_ptr<GLUniform<glm::vec3>> diffuse;
        std::unique_ptr<GLUniform<glm::vec3>> specular;
        std::unique_ptr<GLUniform<float>> opacity;
    public:
        
        GLFlatMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, double _opacity)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/flat.geom");
            
            ambient = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("ambientMat"), _ambient));
            diffuse = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("diffuseMat"), _diffuse));
            specular = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("specMat"), _specular));
            opacity = std::unique_ptr<GLUniform<float>>(new GLUniform<float>(get_uniform_location("opacity"), _opacity));
            test_depth = _opacity >= 0.999;
        }
        
        void pre_draw();
    };
    
    class GLTextureMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture> texture;
        std::unique_ptr<GLUniform<int>> texture_id_uniform;
        std::shared_ptr<int> texture_id = std::make_shared<int>(0);
    public:
        
        GLTextureMaterial(std::shared_ptr<GLTexture> _texture) : texture(_texture)
        {
            shader = std::make_shared<GLShader>("shaders/texture.vert",  "shaders/texture.frag");
            texture_id_uniform = std::unique_ptr<GLUniform<int>>(new GLUniform<int>(get_uniform_location("texture0"), texture_id));
        }
        
        void pre_draw();
    };
    
    class GLSkyboxMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture3D> texture;
        std::unique_ptr<GLUniform<int>> texture_id_uniform;
        std::shared_ptr<int> texture_id = std::make_shared<int>(0);
    public:
        
        GLSkyboxMaterial(std::shared_ptr<GLTexture3D> _texture) : texture(_texture)
        {
            shader = std::make_shared<GLShader>("shaders/skybox.vert",  "shaders/skybox.frag");
            texture_id_uniform = std::unique_ptr<GLUniform<int>>(new GLUniform<int>(get_uniform_location("texture0"), texture_id));
        }
        
        void pre_draw();
    };
    
    class GLGrassMaterial : public GLMaterial
    {
        std::unique_ptr<GLUniform<glm::vec3>> ambient;
        std::unique_ptr<GLUniform<glm::vec3>> diffuse;
        std::unique_ptr<GLUniform<float>> opacity;
    public:
        
        GLGrassMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, double _opacity)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom");
            cull_back_faces = false;
            
            ambient = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("ambientMat"), _ambient));
            diffuse = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("diffuseMat"), _diffuse));
            opacity = std::unique_ptr<GLUniform<float>>(new GLUniform<float>(get_uniform_location("opacity"), _opacity));
            test_depth = _opacity >= 0.999;
        }
        
        void pre_draw();
    };
    
    class GLSpiderLegsMaterial : public GLMaterial
    {
        std::unique_ptr<GLUniform<glm::vec3>> ambient;
        std::unique_ptr<GLUniform<glm::vec3>> diffuse;
        std::unique_ptr<GLUniform<glm::vec3>> specular;
        std::unique_ptr<GLUniform<float>> opacity;
    public:
        
        GLSpiderLegsMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, double _opacity)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/spider_legs.geom");
            
            ambient = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("ambientMat"), _ambient));
            diffuse = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("diffuseMat"), _diffuse));
            specular = std::unique_ptr<GLUniform<glm::vec3>>(new GLUniform<glm::vec3>(get_uniform_location("specMat"), _specular));
            opacity = std::unique_ptr<GLUniform<float>>(new GLUniform<float>(get_uniform_location("opacity"), _opacity));
            test_depth = _opacity >= 0.999;
        }
        
        void pre_draw()
        {
            GLMaterial::pre_draw();
            ambient->use();
            diffuse->use();
            specular->use();
            opacity->use();
        }
    };
    
    class GLFogMaterial : public GLMaterial
    {
        
    public:
        GLFogMaterial()
        {
            shader = std::make_shared<GLShader>("shaders/fog.vert",  "shaders/fog.frag", "shaders/particle.geom");
            test_depth = false;
        }
    };
}
