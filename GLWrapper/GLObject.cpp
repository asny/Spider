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

GLObject::GLObject(vector<VertexAttribute> _attributes, shared_ptr<GLMaterial> _material, GLenum _drawmode)
: attributes(_attributes), material(_material), drawmode(_drawmode)
{
    // Generate array and buffer
    glGenVertexArrays(1, &array_id);
    glGenBuffers(1, &buffer_id);
    
    // Bind array and buffer
    glBindVertexArray(array_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    
    // Compute the stride
    for (auto attribute : attributes)
    {
        stride += attribute.size;
    }
    
    // Initialize vertex attributes
    int start_index = 0;
    for (auto attribute : attributes)
    {
        GLuint location = material->get_attribute_location(attribute.name);
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, attribute.size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const GLvoid *)(start_index * sizeof(float)));
        start_index += attribute.size;
    }
    
    check_gl_error();
}

GLObject::GLObject(std::shared_ptr<Geometry> _geometry, std::shared_ptr<GLMaterial> _material, GLenum _drawmode) : material(_material), geometry(_geometry), drawmode(_drawmode)
{
    // Generate and bind array
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    auto used_vec2_attributes = get_used_attributes(geometry->get_vec2_vertex_attributes());
    auto used_vec3_attributes = get_used_attributes(geometry->get_vec3_vertex_attributes());
    
    no_vertices = used_vec3_attributes.front()->get_size();
    int buffer_index = 0;
    // Generate buffers
    glGenBuffers(static_cast<int>(used_vec2_attributes.size() + used_vec3_attributes.size()), buffer_ids);
    
    // Initialize vec2 vertex attributes
    for (auto attribute : used_vec2_attributes)
    {
        // Bind buffer
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_index]);
        buffer_index++;
        
        material->initialize_vertex_attribute(attribute->get_id(), 2);
    }
    
    // Initialize vec3 vertex attributes
    for (auto attribute : used_vec3_attributes)
    {
        // Bind buffer
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_index]);
        buffer_index++;
        
        material->initialize_vertex_attribute(attribute->get_id(), 3);
    }
    update_vertex_attributes();
    
    check_gl_error();
}

void GLObject::update_vertex_attributes()
{
    auto used_vec2_attributes = get_used_attributes(geometry->get_vec2_vertex_attributes());
    auto used_vec3_attributes = get_used_attributes(geometry->get_vec3_vertex_attributes());
    
    int buffer_index = 0;
    // Send vec2 vertex attributes
    for (auto attribute : used_vec2_attributes)
    {
        auto data = std::vector<float>(2 * attribute->get_size());
        int i = 0;
        for(auto vertexId = geometry->vertices_begin(); vertexId != geometry->vertices_end(); vertexId++)
        {
            auto vec = attribute->get_value(vertexId);
            data[i * 2] = vec.x;
            data[i * 2 + 1] = vec.y;
            i++;
        }
        
        // Bind buffer and send data
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_index]);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        buffer_index++;
    }
    
    // Send vec3 vertex attributes
    for (auto attribute : used_vec3_attributes)
    {
        auto data = std::vector<float>(3 * attribute->get_size());
        int i = 0;
        for(auto vertexId = geometry->vertices_begin(); vertexId != geometry->vertices_end(); vertexId++)
        {
            auto vec = attribute->get_value(vertexId);
            data[i * 3] = vec.x;
            data[i * 3 + 1] = vec.y;
            data[i * 3 + 2] = vec.z;
            i++;
        }
        
        // Bind buffer and send data
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_index]);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        buffer_index++;
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

void GLObject::draw(const mat4& viewMatrix, const mat4& projectionMatrix)
{
    if(no_vertices != 0)
    {   
        material->PreDrawing();
        material->set(modelMatrix, viewMatrix, projectionMatrix);
        
        glBindVertexArray(array_id);
        glDrawArrays(drawmode, 0, no_vertices);
        
        check_gl_error();
    }
}
