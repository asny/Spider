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

GLObject::GLObject(std::shared_ptr<Mesh> _geometry, std::shared_ptr<GLMaterial> _material) : material(_material), geometry(_geometry)
{
    // Generate and bind array
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    vec3_vertex_attributes.push_back(material->create_attribute("position", geometry->position()));
    material->create_attributes(vec2_vertex_attributes, vec3_vertex_attributes);
}

void GLObject::draw()
{
    // Infer draw mode
    GLenum drawmode;
    int no_vertices;
    if(geometry->get_no_faces() > 0)
    {
        drawmode = GL_TRIANGLES;
        no_vertices = geometry->get_no_faces() * 3;
    }
    else if(geometry->get_no_edges() > 0)
    {
        drawmode = GL_LINES;
        no_vertices = geometry->get_no_edges() * 2;
    }
    else if(geometry->get_no_vertices() > 0)
    {
        drawmode = GL_POINTS;
        no_vertices = geometry->get_no_vertices();
    }
    else
    {
        return;
    }
    
    // Update buffers if necessary
    for (auto glAttribute : vec2_vertex_attributes)
    {
        update_attribute(glAttribute);
    }
    
    for (auto glAttribute : vec3_vertex_attributes)
    {
        update_attribute(glAttribute);
    }
    
    // Use material specific uniforms and states
    material->pre_draw();
    
    // Bind vertex array and draw
    glBindVertexArray(array_id);
    glDrawArrays(drawmode, 0, no_vertices);
    
    check_gl_error();
}
