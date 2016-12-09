//
//  GLShader.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 07/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GLShader.hpp"

using namespace oogl;
using namespace std;

map<string, shared_ptr<GLShader>> GLShader::dictionary = map<string, shared_ptr<GLShader>>();
GLuint GLShader::current_shader_id = -1;
