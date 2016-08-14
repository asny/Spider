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
    
    for (auto attributeRecipe : material->get_vertex_attributes())
    {
        if(attributeRecipe.size == 2)
        {
            auto attribute = geometry->get_vec2_vertex_attribute(attributeRecipe.name);
            vec2_vertex_attributes.push_back(GLVertexAttribute<vec2>(attributeRecipe.name, attributeRecipe.size, attributeRecipe.location, attribute));
        }
        else if(attributeRecipe.size == 3)
        {
            auto attribute = geometry->get_vec3_vertex_attribute(attributeRecipe.name);
            vec3_vertex_attributes.push_back(GLVertexAttribute<vec3>(attributeRecipe.name, attributeRecipe.size, attributeRecipe.location, attribute));
        }
    }
}

void GLObject::draw(const mat4& viewMatrix, const mat4& projectionMatrix)
{
    // TODO: Depends on the drawmode
    int no_vertices = geometry->get_no_faces() * 3;
    if(no_vertices != 0)
    {
        for (auto glAttribute : vec2_vertex_attributes)
        {
            glAttribute.update(geometry);
        }
        
        for (auto glAttribute : vec3_vertex_attributes)
        {
            glAttribute.update(geometry);
        }
        
        material->pre_draw();
        material->set(modelMatrix, viewMatrix, projectionMatrix);
        
        glBindVertexArray(array_id);
        glDrawArrays(drawmode, 0, no_vertices);
        
        check_gl_error();
    }
}
