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
    class GLVertexAttribute
    {
    public:
        GLVertexAttribute(std::string _name, int _size, std::shared_ptr<GLShader> shader) : name(_name), size(_size)
        {
            // Generate and bind buffer
            glGenBuffers(1, &buffer_id);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            
            // Initialize attribute
            GLuint location = shader->get_attribute_location(name);
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, size * sizeof(float), (const GLvoid *)(0));
            check_gl_error();
        }
        
        template<class T>
        void update_data(const std::vector<T>& data)
        {
            auto floats = std::vector<float>(size * data.size());
            for(int i = 0; i < data.size(); i++)
            {
                const T& vec = data.at(i);
                for(int j = 0; j < size; j++)
                {
                    floats[i * size + j] = vec[j];
                }
            }
            // Bind buffer and send data
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            glBufferData(GL_ARRAY_BUFFER, floats.size() * sizeof(float), &floats[0], GL_STATIC_DRAW);
            check_gl_error();
        }
        
        const std::string& get_name()
        {
            return name;
        }
        
    private:
        GLuint buffer_id;
        std::string name;
        int size;
    };
    
    
    class GLMaterial
    {
        static bool currently_cull_back_faces;
        
    protected:
        bool cull_back_faces = true;
        
        std::shared_ptr<GLShader> shader;
        
    public:
        
        virtual void PreDrawing();
        
        /**
         Updates the standard matrices.
         */
        virtual void set(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
        {
            // TODO: Only set if necessary
            glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
            shader->set_uniform_variable_if_defined("MVMatrix", modelViewMatrix);
            shader->set_uniform_variable_if_defined("NMatrix", inverseTranspose(modelViewMatrix));
            shader->set_uniform_variable_if_defined("PMatrix", projectionMatrix);
            shader->set_uniform_variable_if_defined("MVPMatrix", projectionMatrix * modelViewMatrix);
        }
        
        virtual std::vector<GLVertexAttribute> get_vertex_attributes()
        {
            return {GLVertexAttribute("position", 3, shader)};
        }
        
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
        
        virtual std::vector<GLVertexAttribute> get_vertex_attributes()
        {
            auto attributes = GLMaterial::get_vertex_attributes();
            attributes.push_back(GLVertexAttribute("normal", 3, shader));
            return attributes;
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
        
        virtual std::vector<GLVertexAttribute> get_vertex_attributes()
        {
            auto attributes = GLMaterial::get_vertex_attributes();
            attributes.push_back(GLVertexAttribute("uv_coordinates", 2, shader));
            return attributes;
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
        
        virtual std::vector<GLVertexAttribute> get_vertex_attributes()
        {
            return GLMaterial::get_vertex_attributes();            
        }
    };
}
