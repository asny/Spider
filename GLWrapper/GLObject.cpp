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
    
    auto attribute_names = material->get_attribute_ids();
    for (auto name : attribute_names)
    {
        initialize_vertex_attribute(name);
    }
    
    update_vertex_attributes();
    check_gl_error();
}

void GLObject::initialize_vertex_attribute(std::string name)
{
    // Check if vertex attribute has already been created.
    if(buffer_ids.find(name) != buffer_ids.end())
    {
        return;
    }
    
    // Determine the size
    int size = 0;
    if(geometry->get_vec2_vertex_attribute(name))
    {
        size = 2;
    }
    else if(geometry->get_vec3_vertex_attribute(name))
    {
        size = 3;
    }
    if(size == 0)
        return;
    
    // Generate and bind buffer
    GLuint buffer_id;
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    buffer_ids[name] = buffer_id;
    
    // Initialize attribute
    material->initialize_vertex_attribute(name, size);
}

void GLObject::update_vertex_attributes()
{
    // TODO: Should be moved to where it is used
    no_vertices = geometry->get_no_vertices();
    
    // Send vertex attributes
    for (auto attributeId : material->get_attribute_ids())
    {
        auto buffer_id = buffer_ids.find(attributeId);
        if(buffer_id != buffer_ids.end())
        {
            std::vector<float> data;
            auto vec2Attribute = geometry->get_vec2_vertex_attribute(attributeId);
            if(vec2Attribute)
            {
                data = std::vector<float>(2 * vec2Attribute->get_size());
                int i = 0;
                for(auto vertexId = geometry->vertices_begin(); vertexId != geometry->vertices_end(); vertexId++)
                {
                    auto vec = vec2Attribute->get_value(vertexId);
                    data[i * 2] = vec.x;
                    data[i * 2 + 1] = vec.y;
                    i++;
                }
            }
            
            auto vec3Attribute = geometry->get_vec3_vertex_attribute(attributeId);
            if(vec3Attribute)
            {
                data = std::vector<float>(3 * vec3Attribute->get_size());
                int i = 0;
                for(auto vertexId = geometry->vertices_begin(); vertexId != geometry->vertices_end(); vertexId++)
                {
                    auto vec = vec3Attribute->get_value(vertexId);
                    data[i * 3] = vec.x;
                    data[i * 3 + 1] = vec.y;
                    data[i * 3 + 2] = vec.z;
                    i++;
                }
            }
            
            // Bind buffer and send data
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id->second);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
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
