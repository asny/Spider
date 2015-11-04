//
//  controller.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "View.h"

#include <GLUT/glut.h>

void display_(){
    View::get_instance()->display();
}

void keyboard_(unsigned char key, int x, int y){
    View::get_instance()->keyboard(key, x, y);
}

void reshape_(int width, int height){
    View::get_instance()->reshape(width, height);
}

void visible_(int v){
    View::get_instance()->visible(v);
}

void animate_(){
    View::get_instance()->animate();
}

View* View::instance = NULL;

static const std::vector<glm::vec3> cube_data = {
    glm::vec3(-1.0f,-1.0f,-1.0f), // triangle 1 : begin
    glm::vec3(-1.0f,-1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f), // triangle 1 : end
    glm::vec3(1.0f, 1.0f,-1.0f), // triangle 2 : begin
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f,-1.0f), // triangle 2 : end
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(1.0f,-1.0f,-1.0f),
    glm::vec3(1.0f, 1.0f,-1.0f),
    glm::vec3(1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f,-1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3(-1.0f,-1.0f, 1.0f),
    glm::vec3(-1.0f,-1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f,-1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f,-1.0f),
    glm::vec3(1.0f, 1.0f,-1.0f),
    glm::vec3(1.0f,-1.0f,-1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f,-1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f,-1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f)
};

View::View(std::shared_ptr<Model> _model, int &argc, char** argv)
{
    instance = this;
    model = _model;
    
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutCreateWindow("Spider");
    glutReshapeWindow(WIN_SIZE_X, WIN_SIZE_Y);
    
    glutDisplayFunc(display_);
    glutKeyboardFunc(keyboard_);
    glutVisibilityFunc(visible_);
    glutReshapeFunc(reshape_);
    glutIdleFunc(animate_);
    
    GLWrapper::initialize();
    create_shaders_and_objects();
    
    cube->get_shader()->use();
    cube->get_shader()->set_uniform_variable("lightPos", light_pos);
    
    grass->get_shader()->use();
    grass->get_shader()->set_uniform_variable("lightPos", light_pos);
    
    terrain->get_shader()->use();
    terrain->get_shader()->set_uniform_variable("lightPos", light_pos);
    
    glutMainLoop();
}

void View::display()
{
    if (glutGet(GLUT_WINDOW_WIDTH) != WIN_SIZE_X || glutGet(GLUT_WINDOW_HEIGHT) != WIN_SIZE_Y) {
        return;
    }
    GLWrapper::initialize_draw();
    
    GLWrapper::cull_backface();
    terrain->draw();
    cube->draw();
    
    GLWrapper::cull_backface(false);
    grass->draw();
    
    glutSwapBuffers();
}

void View::reshape(int width, int height)
{
    WIN_SIZE_X = width;
    WIN_SIZE_Y = height;
    GLWrapper::set_screen_size(width, height);
    
    glutPostRedisplay();
}

void View::animate()
{
    GLfloat timeValue = glutGet(GLUT_ELAPSED_TIME)*0.002;
    glm::vec3 animation(0.5 * sin(timeValue) + 0.5, 0., 0.);
    cube->set_model_matrix(glm::translate(glm::mat4(), animation));
    
    GLWrapper::set_view(model->get_spider_position(), model->get_spider_view_direction());
    
    grass->get_shader()->use();
    grass->get_shader()->set_uniform_variable("spiderPosition", model->get_spider_position());
    grass->get_shader()->set_uniform_variable("wind", animation);
    
    if(model->terrain_needs_update())
    {
        std::vector<glm::vec3> terrain_positions, terrain_normals, grass_end_points;
        model->get_terrain(terrain_positions, terrain_normals, grass_end_points);
        
        terrain->set_vertex_attribute("position", terrain_positions);
        terrain->set_vertex_attribute("normal", terrain_normals);
        terrain->finalize_vertex_attributes();
        
        grass->set_vertex_attribute("end_point", grass_end_points);
        grass->finalize_vertex_attributes();
    }
    
    glutPostRedisplay();
}

void View::keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case '\033':
            exit(0);
            break;
        case 'w':
            model->move_forward();
            break;
        case 'a':
            model->rotate_left();
            break;
        case 'd':
            model->rotate_right();
            break;
        case 's':
            model->move_backwards();
            break;
        case ' ':
            if(!CONTINUOUS)
            {
                std::cout << "MOTION STARTED" << std::endl;
            }
            else {
                std::cout << "MOTION PAUSED" << std::endl;
            }
            CONTINUOUS = !CONTINUOUS;
            break;
    }
}

void View::visible(int v)
{
    if(v==GLUT_VISIBLE)
        glutIdleFunc(animate_);
    else
        glutIdleFunc(0);
}

void View::create_shaders_and_objects()
{
    // Create shaders
    auto phong_shader = std::shared_ptr<GLShader>(new GLShader("shaders/phong.vert",  "shaders/phong.frag"));
    auto grass_shader = std::shared_ptr<GLShader>(new GLShader("shaders/grass.vert",  "shaders/phong.frag", "shaders/grass.geom"));
    auto fastphong_shader = std::shared_ptr<GLShader>(new GLShader("shaders/fastphong.vert",  "shaders/phong.frag", "shaders/fastphong.geom"));
    
    // cube
    auto material = GLMaterial {{0.15f,0.15f,0.15f, 1.f}, {0.4f, 0.2f, 0.6f, 1.f}, {0.2f, 0.2f, 0.8f, 1.f}};
    cube = std::unique_ptr<GLObject>(new GLObject(fastphong_shader, material));
    
    cube->initialize_vertex_attributes({"position"});
    cube->set_vertex_attribute("position", cube_data);
    cube->finalize_vertex_attributes();
    
    // terrain
    material = GLMaterial {{0.25f,0.25f,0.25f, 1.f}, {0.4f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
    terrain = std::unique_ptr<GLObject>(new GLObject(phong_shader, material, GL_TRIANGLE_STRIP));
    terrain->initialize_vertex_attributes({"position", "normal"});
    
    // Grass
    material = GLMaterial {{0.2f,0.2f,0.f, 1.f}, {0.2f, 0.4f, 0.f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
    grass = std::unique_ptr<GLObject>(new GLObject(grass_shader, material, GL_LINES));
    grass->initialize_vertex_attributes({"end_point"});
}
