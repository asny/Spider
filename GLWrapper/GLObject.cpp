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
    
    for (auto glAttribute : vertex_attributes)
    {
        if(glAttribute.get_size() == 2)
        {
            auto attribute = geometry->get_vec2_vertex_attribute(glAttribute.get_name());
            using namespace std::placeholders;
            std::function<void(Attribute<VertexID, vec2>*)> callback = std::bind(&GLObject::update_vertex_vec2_attribute, this, _1, glAttribute);
            callback(attribute.get());
            attribute->subscribe(callback);
        }
        else if(glAttribute.get_size() == 3)
        {
            auto attribute = geometry->get_vec3_vertex_attribute(glAttribute.get_name());
            using namespace std::placeholders;
            std::function<void(Attribute<VertexID, vec3>*)> callback = std::bind(&GLObject::update_vertex_vec3_attribute, this, _1, glAttribute);
            callback(attribute.get());
            attribute->subscribe(callback);
        }
    }
}

void GLObject::update_vertex_vec2_attribute(Attribute<VertexID, vec2>* attribute, GLVertexAttribute& glAttribute)
{
    update_vertex_attribute(attribute, glAttribute);
}

void GLObject::update_vertex_vec3_attribute(Attribute<VertexID, vec3>* attribute, GLVertexAttribute& glAttribute)
{
    update_vertex_attribute(attribute, glAttribute);
}

void GLObject::draw(const mat4& viewMatrix, const mat4& projectionMatrix)
{
    // TODO: Depends on the drawmode
    int no_vertices = geometry->get_no_faces() * 3;
    if(no_vertices != 0)
    {   
        material->pre_draw();
        material->set(modelMatrix, viewMatrix, projectionMatrix);
        
        glBindVertexArray(array_id);
        glDrawArrays(drawmode, 0, no_vertices);
        
        check_gl_error();
    }
}
