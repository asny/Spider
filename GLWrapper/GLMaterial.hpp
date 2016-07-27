//
//  GLMaterial.hpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 27/07/16.
//  Copyright © 2016 Asger Nyman Christiansen. All rights reserved.
//

#pragma once

#include "GLUtility.h"
#include "GLShader.hpp"
#include "GLTexture.hpp"

namespace oogl
{
    class GLMaterial
    {
        static bool currently_cull_back_faces;
        
    protected:
        bool cull_back_faces = true;
        
        std::shared_ptr<GLShader> shader;
        
    public:
        virtual void PreDrawing();
        
        /**
         TODO: Should be deleted!
         Updates the value of the uniform variable with the given name.
         */
        template<typename T>
        void set_uniform_variable(std::string name, const T& value)
        {
            shader->set_uniform_variable(name, value);
        }
        
        /**
         TODO: Should be deleted!
         */
        GLuint get_attribute_location(std::string variable_name)
        {
            return shader->get_attribute_location(variable_name);
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
            shader = std::shared_ptr<GLShader>(new GLShader("shaders/phong.vert",  "shaders/phong.frag"));
        }
        
        void PreDrawing();
    };
    
    class GLTextureMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture> texture;
    public:
        
        GLTextureMaterial(std::shared_ptr<GLTexture> _texture) : texture(_texture)
        {
            shader = std::shared_ptr<GLShader>(new GLShader("shaders/texture.vert",  "shaders/texture.frag"));
        }
        
        void PreDrawing();
    };
    
    class GLSkyboxMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture3D> texture;
    public:
        
        GLSkyboxMaterial(std::shared_ptr<GLTexture3D> _texture) : texture(_texture)
        {
            shader = std::shared_ptr<GLShader>(new GLShader("shaders/skybox.vert",  "shaders/skybox.frag"));
        }
        
        void PreDrawing();
    };
    
    class GLGrassMaterial : public GLMaterial
    {
        glm::vec4 ambient;
        glm::vec4 diffuse;
    public:
        
        GLGrassMaterial(glm::vec4 _ambient, glm::vec4 _diffuse) : ambient(_ambient), diffuse(_diffuse)
        {
            shader = std::shared_ptr<GLShader>(new GLShader("shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom"));
            cull_back_faces = false;
        }
        
        void PreDrawing();
    };
    
    class GLSpiderLegsMaterial : public GLStandardMaterial
    {
    public:
        
        GLSpiderLegsMaterial(glm::vec4 _ambient, glm::vec4 _diffuse, glm::vec4 _specular) : GLStandardMaterial(_ambient, _diffuse, _specular)
        {
            shader = std::shared_ptr<GLShader>(new GLShader("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/spider_legs.geom"));
        }
    };
}
