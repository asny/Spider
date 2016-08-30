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
        
        /**
         TODO: Should be deleted!
         Updates the value of the uniform variable with the given name.
         */
        template<typename T>
        void set_uniform_variable(std::string name, const T& value)
        {
            shader->set_uniform_variable(name, value);
        }
    };
    
    class GLStandardMaterial : public GLMaterial
    {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
    public:
        
        GLStandardMaterial(glm::vec4 _ambient, glm::vec4 _diffuse, glm::vec4 _specular) :
        ambient(_ambient), diffuse(_diffuse), specular(_specular)
        {
            shader = std::make_shared<GLShader>("shaders/phong.vert",  "shaders/phong.frag");
        }
        
        void pre_draw();
    };
    
    class GLTextureMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture> texture;
    public:
        
        GLTextureMaterial(std::shared_ptr<GLTexture> _texture) : texture(_texture)
        {
            shader = std::make_shared<GLShader>("shaders/texture.vert",  "shaders/texture.frag");
        }
        
        void pre_draw();
    };
    
    class GLSkyboxMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture3D> texture;
    public:
        
        GLSkyboxMaterial(std::shared_ptr<GLTexture3D> _texture) : texture(_texture)
        {
            shader = std::make_shared<GLShader>("shaders/skybox.vert",  "shaders/skybox.frag");
        }
        
        void pre_draw();
    };
    
    class GLGrassMaterial : public GLMaterial
    {
        glm::vec4 ambient;
        glm::vec4 diffuse;
    public:
        
        GLGrassMaterial(glm::vec4 _ambient, glm::vec4 _diffuse) : ambient(_ambient), diffuse(_diffuse)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom");
            cull_back_faces = false;
        }
        
        void pre_draw();
    };
    
    class GLSpiderLegsMaterial : public GLStandardMaterial
    {
    public:
        
        GLSpiderLegsMaterial(glm::vec4 _ambient, glm::vec4 _diffuse, glm::vec4 _specular) : GLStandardMaterial(_ambient, _diffuse, _specular)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/spider_legs.geom");
        }
    };
}
