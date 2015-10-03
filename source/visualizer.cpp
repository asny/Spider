//
//  visualizer.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "visualizer.h"
#include "gtc/matrix_inverse.hpp"

Visualizer::GLObject::GLObject(GLuint _shader, const glm::vec4& ambient_mat_, const glm::vec4& diffuse_mat_, const glm::vec4& specular_mat_) : shader(_shader), ambient_mat(ambient_mat_), diffuse_mat(diffuse_mat_), specular_mat(specular_mat_)
{
    // Generate arrays and buffers
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);
    
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    
    // Initialize shader attributes
    position_att = glGetAttribLocation(shader, "position");
    if (position_att == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'position' attribute." << std::endl;
    }
    vector_att = glGetAttribLocation(shader, "vector");
    if (vector_att == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'vector' attribute." << std::endl;
    }
    
    glEnableVertexAttribArray(position_att);
    glEnableVertexAttribArray(vector_att);
    check_gl_error();
}

void Visualizer::GLObject::add_data(std::vector<glm::vec3> _data)
{
    data = std::vector<glm::vec3>(_data);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*data.size(), &data[0], GL_STATIC_DRAW);
}

void Visualizer::GLObject::draw(GLenum mode)
{
    if(data.size() != 0)
    {
        glUseProgram(shader);
        GLuint uniform = (GLuint) glGetUniformLocation(shader, "ambientMat");
        if (uniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'ambientMat' uniform."<<std::endl;
        }
        glUniform4fv(uniform, 1, &ambient_mat[0]);
        
        uniform = (GLuint) glGetUniformLocation(shader, "diffuseMat");
        if (uniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'diffuseMat' uniform."<<std::endl;
        }
        glUniform4fv(uniform, 1, &diffuse_mat[0]);
        
        uniform = (GLuint) glGetUniformLocation(shader, "specMat");
        if (uniform == NULL_LOCATION) {
            std::cerr << "Shader did not contain the 'specMat' uniform."<<std::endl;
        }
        glUniform4fv(uniform, 1, &specular_mat[0]);
        
        glUseProgram(shader);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        
        glVertexAttribPointer(position_att, 3, GL_DOUBLE, GL_FALSE, 2*sizeof(glm::vec3), (const GLvoid *)0);
        glVertexAttribPointer(vector_att, 3, GL_DOUBLE, GL_FALSE, 2*sizeof(glm::vec3), (const GLvoid *)sizeof(glm::vec3));
        
        glDrawArrays(mode, 0, static_cast<int>(data.size())/2);
        check_gl_error();
    }
}

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile)
{
    FILE *filePointer;
    char *content = NULL;
    
    int count=0;
    
    if (shaderFile != NULL) {
        filePointer = fopen(shaderFile,"rt");
        
        if (filePointer != NULL) {
            
            fseek(filePointer, 0, SEEK_END);
            count = static_cast<int>(ftell(filePointer));
            rewind(filePointer);
            
            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = static_cast<int>(fread(content,sizeof(char),count,filePointer));
                content[count] = '\0';
            }
            fclose(filePointer);
        }
    }
    return content;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint Visualizer::init_shader(const char* vShaderFile, const char* fShaderFile, const char* outputAttributeName, const char* gShaderFile)
{
    struct Shader {
        const char*  filename;
        GLenum       type;
        GLchar*      source;
    };
    std::vector<Shader> shaders = { { vShaderFile, GL_VERTEX_SHADER, NULL }, { fShaderFile, GL_FRAGMENT_SHADER, NULL } };
    if(gShaderFile)
    {
        shaders.push_back({ gShaderFile, GL_GEOMETRY_SHADER, NULL });
    }
    
    GLuint program = glCreateProgram();
    
    for ( int i = 0; i < shaders.size(); ++i ) {
        Shader& s = shaders[i];
        s.source = readShaderSource( s.filename );
        if ( shaders[i].source == NULL ) {
            std::cerr << "Failed to read " << s.filename << std::endl;
            exit( EXIT_FAILURE );
        }
        GLuint shader = glCreateShader( s.type );
        glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
        glCompileShader( shader );
        
        GLint  compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            std::cerr << s.filename << " failed to compile:" << std::endl;
            GLint  logSize;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
            char* logMsg = new char[logSize];
            glGetShaderInfoLog( shader, logSize, NULL, logMsg );
            std::cerr << logMsg << std::endl;
            delete [] logMsg;
            
            exit( EXIT_FAILURE );
        }
        
        delete [] s.source;
        
        glAttachShader( program, shader );
    }
    
    /* Link output */
    glBindFragDataLocation(program, 0, outputAttributeName);
    
    /* link  and error check */
    glLinkProgram(program);
    
    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        std::cerr << "Shader program failed to link" << std::endl;
        GLint  logSize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
        char* logMsg = new char[logSize];
        glGetProgramInfoLog( program, logSize, NULL, logMsg );
        std::cerr << logMsg << std::endl;
        delete [] logMsg;
        
        exit( EXIT_FAILURE );
    }
    
    /* use program object */
    glUseProgram(program);
    
    return program;
}

Visualizer::Visualizer(const glm::vec3& light_pos)
{
    // Initialize shaders
    gouraud_shader = init_shader("shaders/gouraud.vert",  "shaders/gouraud.frag", "fragColour");
    
    // Send light position uniform to the shaders
    glUseProgram(gouraud_shader);
    glm::vec3 lp(light_pos);
    GLuint lightPosUniform = glGetUniformLocation(gouraud_shader, "lightPos");
    if (lightPosUniform == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'lightPos' uniform."<<std::endl;
    }
    glUniform3fv(lightPosUniform, 1, &lp[0]);
    
    check_gl_error();
    
    interface = std::unique_ptr<GLObject>(new GLObject(gouraud_shader, {0.15f,0.4f,0.5f, 1.f}, {0.2f, 0.3f, 0.4f, 1.f}, {0.2f, 0.3f, 0.4f, 1.f}));
    
    // Enable states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    check_gl_error();
}

void Visualizer::reshape(int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    projectionMatrix = glm::perspective(53.f, width/float(height), 1.f, 1000000.f);
    glViewport(0, 0, width, height);
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
    
    glUseProgram(gouraud_shader);
    GLuint MVPMatrixUniform = glGetUniformLocation(gouraud_shader, "MVPMatrix");
    if (MVPMatrixUniform == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'MVPMatrix' uniform."<<std::endl;
    }
    glUniformMatrix4fv(MVPMatrixUniform, 1, GL_TRUE, &modelViewProjectionMatrix[0][0]);
    
    check_gl_error();
}

void Visualizer::set_view_position(glm::vec3 pos)
{
    viewMatrix = glm::lookAt(glm::vec3(pos), center, glm::vec3(0., 1., 0.));
    glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
    glm::mat4 normalMatrix = glm::inverseTranspose(modelViewMatrix);
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
    
    glUseProgram(gouraud_shader);
    GLuint MVMatrixUniform = glGetUniformLocation(gouraud_shader, "MVMatrix");
    if (MVMatrixUniform == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'MVMatrix' uniform."<<std::endl;
    }
    glUniformMatrix4fv(MVMatrixUniform, 1, GL_TRUE, &modelViewMatrix[0][0]);
    
    GLuint NormalMatrixUniform = glGetUniformLocation(gouraud_shader, "NormalMatrix");
    if (NormalMatrixUniform == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'NormalMatrix' uniform."<<std::endl;
    }
    glUniformMatrix4fv(NormalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
    
    GLuint MVPMatrixUniform = glGetUniformLocation(gouraud_shader, "MVPMatrix");
    if (MVPMatrixUniform == NULL_LOCATION) {
        std::cerr << "Shader did not contain the 'MVPMatrix' uniform."<<std::endl;
    }
    glUniformMatrix4fv(MVPMatrixUniform, 1, GL_TRUE, &modelViewProjectionMatrix[0][0]);
    
    check_gl_error();
}

void Visualizer::draw()
{
    glClearColor(1., 1., 1., 0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    interface->draw();
    
    check_gl_error();
}

void Visualizer::update()
{
    interface->clear_data();
    
}
