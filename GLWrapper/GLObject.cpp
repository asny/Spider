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

GLObject::GLObject(std::shared_ptr<GLShader> _shader, const GLMaterial& _material, GLenum _drawmode, std::shared_ptr<GLTexture> _texture)
: shader(_shader), material(_material), texture(_texture), drawmode(_drawmode)
{
    // Generate arrays and buffers
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    
    if(texture)
    {
        create_vertex_attribute("uv_coordinates", 2);
    }
    
    check_gl_error();
}

void GLObject::create_vertex_attribute(string name, int size)
{
    GLuint location = shader->get_attribute_location(name);
    glEnableVertexAttribArray(location);
    
    attributes.insert( {name, VertexAttribute{ size, current_start_index }} );
    current_start_index += size;
    
    stride += size;
}

void GLObject::set_vertex_attribute(std::string name, const std::vector<glm::vec2>& _data)
{
    no_vertices = static_cast<int>(_data.size());
    int start_index = attributes.at(name).start_index;
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
    int start_index = attributes.at(name).start_index;
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
    if(texture)
    {
        set_vertex_attribute("uv_coordinates", texture->uv_coordinates);
    }
    
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
            texture->use();
        }
        else {
            shader->set_uniform_variable("ambientMat", material.ambient);
            shader->set_uniform_variable("diffuseMat", material.diffuse);
            shader->set_uniform_variable("specMat", material.specular);
        }
        shader->update_draw_matrices(modelMatrix);
        
        glBindVertexArray(array_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        
        for (auto attribute_name_value_pair : attributes)
        {
            auto attribute = attribute_name_value_pair.second;
            GLuint location = shader->get_attribute_location(attribute_name_value_pair.first);
            glVertexAttribPointer(location, attribute.size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const GLvoid *)(attribute.start_index * sizeof(float)));
        }
        
        glDrawArrays(drawmode, 0, no_vertices);
        
        check_gl_error();
    }
}
