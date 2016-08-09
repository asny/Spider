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
    
    vertex_attributes = material->get_vertex_attributes();
    
    update_vertex_attributes();
    check_gl_error();
}

void GLObject::update_vertex_attributes()
{
    // TODO: Should be moved to where it is used
    no_vertices = geometry->get_no_vertices();
    
    // Send vertex attributes
    for (auto attribute : vertex_attributes)
    {
        std::vector<float> data;
        auto vec2Attribute = geometry->get_vec2_vertex_attribute(attribute.get_name());
        if(vec2Attribute)
        {
            auto data = std::vector<vec2>();
            for(auto vertexId = geometry->vertices_begin(); vertexId != geometry->vertices_end(); vertexId++)
            {
                data.push_back(vec2Attribute->get_value(vertexId));
            }
            attribute.update_data(data);
        }
        
        auto vec3Attribute = geometry->get_vec3_vertex_attribute(attribute.get_name());
        if(vec3Attribute)
        {
            auto data = std::vector<vec3>();
            for(auto vertexId = geometry->vertices_begin(); vertexId != geometry->vertices_end(); vertexId++)
            {
                data.push_back(vec3Attribute->get_value(vertexId));
            }
            attribute.update_data(data);
        }
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
