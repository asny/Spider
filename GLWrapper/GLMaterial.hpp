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
        
        std::vector<GLUniform<int>> int_uniforms;
        std::vector<GLUniform<float>> float_uniforms;
        std::vector<GLUniform<glm::vec2>> vec2_uniforms;
        std::vector<GLUniform<glm::vec3>> vec3_uniforms;
        std::vector<GLUniform<glm::vec4>> vec4_uniforms;
        std::vector<GLUniform<glm::mat4>> mat4_uniforms;
        
        GLuint get_uniform_location(const std::string& name)
        {
            return shader->get_uniform_location(name);
        }
        
    public: // Should be protrected
        void use_uniform_int(const std::string& name, const std::shared_ptr<int> value)
        {
            auto location = get_uniform_location(name);
            if(location != NULL_LOCATION)
                int_uniforms.push_back(GLUniform<int>(location, value));
        }
        
        void use_uniform(const std::string& name, const std::shared_ptr<float> value)
        {
            auto location = get_uniform_location(name);
            if(location != NULL_LOCATION)
                float_uniforms.push_back(GLUniform<float>(location, value));
        }
        
        void use_uniform(const std::string& name, const std::shared_ptr<glm::vec2> value)
        {
            auto location = get_uniform_location(name);
            if(location != NULL_LOCATION)
                vec2_uniforms.push_back(GLUniform<glm::vec2>(location, value));
        }
        
        void use_uniform(const std::string& name, const std::shared_ptr<glm::vec3> value)
        {
            auto location = get_uniform_location(name);
            if(location != NULL_LOCATION)
                vec3_uniforms.push_back(GLUniform<glm::vec3>(location, value));
        }
        
        void use_uniform(const std::string& name, const std::shared_ptr<glm::vec4> value)
        {
            auto location = get_uniform_location(name);
            if(location != NULL_LOCATION)
                vec4_uniforms.push_back(GLUniform<glm::vec4>(location, value));
        }
        
        void use_uniform(const std::string& name, const std::shared_ptr<glm::mat4> value)
        {
            auto location = get_uniform_location(name);
            if(location != NULL_LOCATION)
                mat4_uniforms.push_back(GLUniform<glm::mat4>(location, value));
        }
        
        void use_uniform_int(const std::string& name, const int& value)
        {
            use_uniform_int(name, std::make_shared<int>(value));
        }
        
        void use_uniform(const std::string& name, const float& value)
        {
            use_uniform(name, std::make_shared<float>(value));
        }
        
        void use_uniform(const std::string& name, const glm::vec2& value)
        {
            use_uniform(name, std::make_shared<glm::vec2>(value));
        }
        
        void use_uniform(const std::string& name, const glm::vec3& value)
        {
            use_uniform(name, std::make_shared<glm::vec3>(value));
        }
        
        void use_uniform(const std::string& name, const glm::vec4& value)
        {
            use_uniform(name, std::make_shared<glm::vec4>(value));
        }
        
        void use_uniform(const std::string& name, const glm::mat4& value)
        {
            use_uniform(name, std::make_shared<glm::mat4>(value));
        }
        
        bool cull_back_faces = true;
        bool test_depth = true;
        
        std::shared_ptr<GLShader> shader;
        
        GLMaterial()
        {
        }
        
    public:
        
        GLuint get_attribute_location(const std::string& name)
        {
            return shader->get_attribute_location(name);
        }
        
        virtual void pre_draw();
        
        void setup_camera(const std::shared_ptr<glm::mat4> modelView, const std::shared_ptr<glm::mat4> inverseModelView, const std::shared_ptr<glm::mat4> projection, const std::shared_ptr<glm::mat4> modelViewProjection, const std::shared_ptr<glm::vec3> position)
        {
            use_uniform("MVMatrix", modelView);
            use_uniform("NMatrix", inverseModelView);
            use_uniform("PMatrix", projection);
            use_uniform("MVPMatrix", modelViewProjection);
            use_uniform("eyePosition", position);
        }
        
    };
    
    class GLStandardMaterial : public GLMaterial
    {
    public:
        
        GLStandardMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, double _opacity)
        {
            shader = std::make_shared<GLShader>("shaders/phong.vert",  "shaders/phong.frag");
            use_uniform("ambientMat", _ambient);
            use_uniform("diffuseMat", _diffuse);
            use_uniform("specMat", _specular);
            use_uniform("opacity", _opacity);
            
            test_depth = _opacity >= 0.999;
        }
    };
    
    class GLFlatMaterial : public GLMaterial
    {
    public:
        
        GLFlatMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, double _opacity)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/flat.geom");
            use_uniform("ambientMat", _ambient);
            use_uniform("diffuseMat", _diffuse);
            use_uniform("specMat", _specular);
            use_uniform("opacity", _opacity);
            
            test_depth = _opacity >= 0.999;
        }
    };
    
    class GLTextureMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture> texture;
        std::shared_ptr<int> texture_id = std::make_shared<int>(0);
    public:
        
        GLTextureMaterial(std::shared_ptr<GLTexture> _texture) : texture(_texture)
        {
            shader = std::make_shared<GLShader>("shaders/texture.vert",  "shaders/texture.frag");
            
            use_uniform_int("texture0", texture_id);
        }
        
        void pre_draw();
    };
    
    class GLSkyboxMaterial : public GLMaterial
    {
        std::shared_ptr<GLTexture3D> texture;
        std::shared_ptr<int> texture_id = std::make_shared<int>(0);
    public:
        
        GLSkyboxMaterial(std::shared_ptr<GLTexture3D> _texture) : texture(_texture)
        {
            shader = std::make_shared<GLShader>("shaders/skybox.vert",  "shaders/skybox.frag");
            
            use_uniform_int("texture0", texture_id);
        }
        
        void pre_draw();
    };
    
    class GLGrassMaterial : public GLMaterial
    {
    public:
        
        GLGrassMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, double _opacity)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom");
            
            use_uniform("ambientMat", _ambient);
            use_uniform("diffuseMat", _diffuse);
            use_uniform("opacity", _opacity);
            
            cull_back_faces = false;
            test_depth = _opacity >= 0.999;
        }
    };
    
    class GLSpiderLegsMaterial : public GLMaterial
    {
    public:
        
        GLSpiderLegsMaterial(const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, double _opacity)
        {
            shader = std::make_shared<GLShader>("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/spider_legs.geom");
            
            use_uniform("ambientMat", _ambient);
            use_uniform("diffuseMat", _diffuse);
            use_uniform("specMat", _specular);
            use_uniform("opacity", _opacity);
            
            test_depth = _opacity >= 0.999;
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
