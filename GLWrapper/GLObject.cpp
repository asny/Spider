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

GLObject::GLObject(std::shared_ptr<Geometry> _geometry, std::shared_ptr<GLMaterial> _material) : material(_material), geometry(_geometry)
{
    // Generate and bind array
    glGenVertexArrays(1, &array_id);
    
    use_attribute("position", geometry->position());
}

void GLObject::draw()
{
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
    
    material->pre_draw();
    
    for (auto glAttribute : vec2_vertex_attributes)
    {
        update_attribute(glAttribute);
    }
    
    for (auto glAttribute : vec3_vertex_attributes)
    {
        update_attribute(glAttribute);
    }
    
    for (auto glUniform : float_uniforms)
    {
        glUniform.use();
    }
    
    for (auto glUniform : vec2_uniforms)
    {
        glUniform.use();
    }
    
    for (auto glUniform : vec3_uniforms)
    {
        glUniform.use();
    }
    
    for (auto glUniform : vec4_uniforms)
    {
        glUniform.use();
    }
    
    for (auto glUniform : mat4_uniforms)
    {
        glUniform.use();
    }
    
    glBindVertexArray(array_id);
    glDrawArrays(drawmode, 0, no_vertices);
    
    check_gl_error();
}
