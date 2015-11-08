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
    
    check_gl_error();
}

void GLObject::initialize_vertex_attributes(std::vector<std::string> attribute_names, std::vector<int> attribute_sizes)
{
    if(texture)
    {
        attribute_names.push_back("uv_coordinates");
        attribute_sizes.push_back(2);
    }
    
    int start_index = 0;
    int stride_index = 0;
    for (auto size : attribute_sizes) {
        stride_index += size;
    }
    
    for (int i = 0; i < attribute_names.size(); i++)
    {
        auto attribute = VertexAttribute(shader, attribute_names[i], start_index, stride_index, attribute_sizes[i]);
        attributes.insert( {attribute_names[i], attribute} );
        start_index += attribute_sizes[i];
    }
}

void GLObject::set_vertex_attribute(std::string attribute_name, const std::vector<glm::vec2>& _data)
{
    no_vertices = static_cast<int>(_data.size());
    attributes.at(attribute_name).set_data(_data, data);
}

void GLObject::set_vertex_attribute(std::string attribute_name, const std::vector<glm::vec3>& _data)
{
    no_vertices = static_cast<int>(_data.size());
    attributes.at(attribute_name).set_data(_data, data);
}

void GLObject::finalize_vertex_attributes()
{
    if(texture)
    {
        set_vertex_attribute("uv_coordinates", texture->uv_coordinates);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    
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
        
        for (auto attribute : attributes)
        {
            attribute.second.use();
        }
        
        glDrawArrays(drawmode, 0, no_vertices);
        
        check_gl_error();
    }
}
