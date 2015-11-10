//
//  GLObject.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 07/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GLObject.hpp"

using namespace oogl;
using namespace std;
using namespace glm;

mat4 GLObject::viewMatrix = mat4(1.);
mat4 GLObject::projectionMatrix = mat4(1.);

GLObject::GLObject(std::shared_ptr<GLShader> _shader, const GLMaterial& _material, GLenum _drawmode, std::shared_ptr<GLTexture> _texture)
: shader(_shader), material(_material), texture(_texture), drawmode(_drawmode)
{
    // Generate arrays and buffers
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    
    shader->initialize_vertex_attributes();
    
    check_gl_error();
}

void GLObject::set_vertex_attribute(std::string name, const std::vector<glm::vec2>& _data)
{
    no_vertices = static_cast<int>(_data.size());
    int start_index = shader->get_attribute_start_index(name);
    int stride = shader->get_attributes_stride();
    data.resize(stride * static_cast<int>(_data.size())); // Make sure that data has the correct size.
    
    for (int i = 0; i < _data.size(); i++)
    {
        glm::vec2 vec = _data[i];
        data[start_index + i*stride] = vec.x;
        data[start_index + i*stride + 1] = vec.y;
    }
}

void GLObject::set_vertex_attribute(std::string name, const std::vector<glm::vec3>& _data)
{
    no_vertices = static_cast<int>(_data.size());
    int start_index = shader->get_attribute_start_index(name);
    int stride = shader->get_attributes_stride();
    data.resize(stride * static_cast<int>(_data.size())); // Make sure that data has the correct size.
    
    for (int i = 0; i < _data.size(); i++)
    {
        glm::vec3 vec = _data[i];
        data[start_index + i*stride] = vec.x;
        data[start_index + i*stride + 1] = vec.y;
        data[start_index + i*stride + 2] = vec.z;
    }
}

void GLObject::finalize_vertex_attributes()
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    
    data.clear();
    check_gl_error();
}

void GLObject::draw()
{
    if(no_vertices != 0)
    {
        shader->use();
        if(texture)
        {
            int texture_id = texture->use();
            shader->set_uniform_variable("texture0", texture_id);
        }
        else {
            shader->set_uniform_variable("ambientMat", material.ambient);
            shader->set_uniform_variable("diffuseMat", material.diffuse);
            shader->set_uniform_variable("specMat", material.specular);
        }
        
        mat4 modelViewMatrix = GLObject::viewMatrix * modelMatrix;
        shader->set_uniform_variable_if_defined("MVMatrix", modelViewMatrix);
        shader->set_uniform_variable_if_defined("NMatrix", inverseTranspose(modelViewMatrix));
        shader->set_uniform_variable_if_defined("PMatrix", GLObject::projectionMatrix);
        shader->set_uniform_variable_if_defined("MVPMatrix", GLObject::projectionMatrix * modelViewMatrix);
        
        glBindVertexArray(array_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        
        shader->use_vertex_attributes();
        
        glDrawArrays(drawmode, 0, no_vertices);
        
        check_gl_error();
    }
}
