//
//  GLObject.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 07/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GLObject.hpp"

using namespace oogl;
using namespace geogo;
using namespace std;
using namespace glm;

GLObject::GLObject(std::shared_ptr<Geometry> _geometry, std::shared_ptr<GLMaterial> _material, GLenum _drawmode) : material(_material), geometry(_geometry), drawmode(_drawmode)
{
    // Generate and bind array
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    vertex_attributes = material->get_vertex_attributes();
    update_vertex_attributes();
}

void GLObject::update_vertex_attributes()
{
    for (auto attribute : vertex_attributes)
    {
        update_vertex_attribute(attribute);
    }
    check_gl_error();
}

void GLObject::update_vertex_attribute(GLVertexAttribute& attribute)
{
    auto vec2Attribute = geometry->get_vec2_vertex_attribute(attribute.get_name());
    if(vec2Attribute)
    {
        auto data = std::vector<vec2>();
        for(auto vertex = geometry->vertices_begin(); vertex != geometry->vertices_end(); vertex = vertex->next())
        {
            data.push_back(vec2Attribute->get_value(*vertex));
        }
        attribute.update_data(data);
    }
    
    auto vec3Attribute = geometry->get_vec3_vertex_attribute(attribute.get_name());
    if(vec3Attribute)
    {
        auto data = std::vector<vec3>();
        for(auto vertex = geometry->vertices_begin(); vertex != geometry->vertices_end(); vertex = vertex->next())
        {
            data.push_back(vec3Attribute->get_value(*vertex));
        }
        attribute.update_data(data);
    }
}

void GLObject::draw(const mat4& viewMatrix, const mat4& projectionMatrix)
{
    int no_vertices = geometry->get_no_vertices();
    if(no_vertices != 0)
    {   
        material->pre_draw();
        material->set(modelMatrix, viewMatrix, projectionMatrix);
        
        glBindVertexArray(array_id);
        glDrawArrays(drawmode, 0, no_vertices);
        
        check_gl_error();
    }
}
