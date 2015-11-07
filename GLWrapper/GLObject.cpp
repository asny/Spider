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

void GLObject::initialize_vertex_attributes(std::vector<std::string> attribute_names)
{
    int start_index = 0;
    int stride_index = static_cast<int>(attribute_names.size()) * 3;
    for (std::string attribute_name : attribute_names)
    {
        auto attribute = VertexAttribute(shader, attribute_name, start_index, stride_index, 3);
        attributes.insert( {attribute_name, attribute} );
        start_index += attribute.length();
    }
}

void GLObject::set_vertex_attribute(std::string attribute_name, const std::vector<glm::vec3>& _data)
{
    no_vertices = static_cast<int>(_data.size());
    attributes.at(attribute_name).set_data(_data, data);
}

void GLObject::finalize_vertex_attributes()
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * VertexAttribute::size_of_type(), &data[0], GL_STATIC_DRAW);
    
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
        shader->set_uniform_variable("ambientMat", material.ambient);
        shader->set_uniform_variable("diffuseMat", material.diffuse);
        shader->set_uniform_variable("specMat", material.specular);
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
