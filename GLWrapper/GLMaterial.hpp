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
        
    protected:
        bool cull_back_faces = true;
        
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
        GLUniform<glm::vec4>* ambient;
        GLUniform<glm::vec4>* diffuse;
        GLUniform<glm::vec4>* specular;
    public:
        
        GLStandardMaterial(const glm::vec4& _ambient, const glm::vec4& _diffuse, const glm::vec4& _specular)
        {
            shader = std::make_shared<GLShader>("shaders/phong.vert",  "shaders/phong.frag");
            
            ambient = new GLUniform<glm::vec4>(get_uniform_location("ambientMat"), _ambient);
            diffuse = new GLUniform<glm::vec4>(get_uniform_location("diffuseMat"), _diffuse);
            specular = new GLUniform<glm::vec4>(get_uniform_location("specMat"), _specular);
        }
        
        ~GLStandardMaterial()
        {
            delete ambient;
            delete diffuse;
            delete specular;
        }
        
        void pre_draw();
    };
    
    class GLTextureMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture> texture;
        GLUniform<int>* texture_id_uniform;
        std::shared_ptr<int> texture_id = std::make_shared<int>(0);
    public:
        
        GLTextureMaterial(std::shared_ptr<GLTexture> _texture) : texture(_texture)
        {
            shader = std::make_shared<GLShader>("shaders/texture.vert",  "shaders/texture.frag");
            texture_id_uniform = new GLUniform<int>(get_uniform_location("texture0"), texture_id);
        }
        
        ~GLTextureMaterial()
        {
            delete texture_id_uniform;
        }
        
        void pre_draw();
    };
    
    class GLSkyboxMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture3D> texture;
        GLUniform<int>* texture_id_uniform;
        std::shared_ptr<int> texture_id = std::make_shared<int>(0);
    public:
        
        GLSkyboxMaterial(std::shared_ptr<GLTexture3D> _texture) : texture(_texture)
        {
            shader = std::make_shared<GLShader>("shaders/skybox.vert",  "shaders/skybox.frag");
            texture_id_uniform = new GLUniform<int>(get_uniform_location("texture0"), texture_id);
        }
        
        ~GLSkyboxMaterial()
        {
            delete texture_id_uniform;
        }
        
        void pre_draw();
    };
    
    class GLGrassMaterial : public GLMaterial
    {
        GLUniform<glm::vec4>* ambient;
        GLUniform<glm::vec4>* diffuse;
    public:
        
        GLGrassMaterial(const glm::vec4& _ambient, const glm::vec4& _diffuse)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom");
            cull_back_faces = false;
            ambient = new GLUniform<glm::vec4>(get_uniform_location("ambientMat"), _ambient);
            diffuse = new GLUniform<glm::vec4>(get_uniform_location("diffuseMat"), _diffuse);
        }
        
        ~GLGrassMaterial()
        {
            delete ambient;
            delete diffuse;
        }
        
        void pre_draw();
    };
    
    class GLSpiderLegsMaterial : public GLMaterial
    {
        GLUniform<glm::vec4>* ambient;
        GLUniform<glm::vec4>* diffuse;
        GLUniform<glm::vec4>* specular;
    public:
        
        GLSpiderLegsMaterial(const glm::vec4& _ambient, const glm::vec4& _diffuse, glm::vec4 _specular)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/spider_legs.geom");
            
            ambient = new GLUniform<glm::vec4>(get_uniform_location("ambientMat"), _ambient);
            diffuse = new GLUniform<glm::vec4>(get_uniform_location("diffuseMat"), _diffuse);
            specular = new GLUniform<glm::vec4>(get_uniform_location("specMat"), _specular);
        }
        
        ~GLSpiderLegsMaterial()
        {
            delete ambient;
            delete diffuse;
            delete specular;
        }
        
        void pre_draw()
        {
            GLMaterial::pre_draw();
            ambient->use();
            diffuse->use();
            specular->use();
        }
    };
}
