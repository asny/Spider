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

bool GLObject::currently_cull_back_faces = true;

GLObject::GLObject(vector<VertexAttribute> _attributes, shared_ptr<GLShader> _shader, const GLMaterial& _material, GLenum _drawmode, std::shared_ptr<GLTexture> _texture, bool _cull_back_faces)
: attributes(_attributes), shader(_shader), material(_material), texture(_texture), drawmode(_drawmode), cull_back_faces(_cull_back_faces)
{
    // Generate arrays and buffers
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    
    for (auto attribute : attributes)
    {
        stride += attribute.size;
        GLuint location = shader->get_attribute_location(attribute.name);
        glEnableVertexAttribArray(location);
    }
    
    check_gl_error();
}

void GLObject::update_vertex_attribute(std::string name, const std::vector<glm::vec2>& _data)
{
    no_vertices = static_cast<int>(_data.size());
    int start_index = get_attribute_start_index(name);
    data.resize(stride * static_cast<int>(_data.size())); // Make sure that data has the correct size.
    
    for (int i = 0; i < _data.size(); i++)
    {
        glm::vec2 vec = _data[i];
        data[start_index + i*stride] = vec.x;
        data[start_index + i*stride + 1] = vec.y;
    }
}

void GLObject::update_vertex_attribute(std::string name, const std::vector<glm::vec3>& _data)
{
    no_vertices = static_cast<int>(_data.size());
    int start_index = get_attribute_start_index(name);
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

void GLObject::use_vertex_attributes()
{
    int start_index = 0;
    for (auto attribute : attributes)
    {
        GLuint location = shader->get_attribute_location(attribute.name);
        glVertexAttribPointer(location, attribute.size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const GLvoid *)(start_index * sizeof(float)));
        start_index += attribute.size;
    }
}

void GLObject::draw(const mat4& viewMatrix, const mat4& projectionMatrix)
{
    if(no_vertices != 0)
    {
        if(currently_cull_back_faces != cull_back_faces)
        {
            if(cull_back_faces)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }
            else {
                glDisable(GL_CULL_FACE);
            }
            currently_cull_back_faces = cull_back_faces;
        }
        
        if(texture)
        {
            int texture_id = texture->use();
            shader->set_uniform_variable("texture0", texture_id);
        }
        else {
            shader->set_uniform_variable("ambientMat", material.ambient);
            shader->set_uniform_variable("diffuseMat", material.diffuse);
            if(material.specular.w != 0)
                shader->set_uniform_variable("specMat", material.specular);
        }
        
        mat4 modelViewMatrix = viewMatrix * modelMatrix;
        shader->set_uniform_variable_if_defined("MVMatrix", modelViewMatrix);
        shader->set_uniform_variable_if_defined("NMatrix", inverseTranspose(modelViewMatrix));
        shader->set_uniform_variable_if_defined("PMatrix", projectionMatrix);
        shader->set_uniform_variable_if_defined("MVPMatrix", projectionMatrix * modelViewMatrix);
        
        glBindVertexArray(array_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        
        use_vertex_attributes();
        
        glDrawArrays(drawmode, 0, no_vertices);
        
        check_gl_error();
    }
}
