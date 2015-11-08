//
//  controller.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "View.h"
#include "OBJLoader.hpp"
#include "gtx/rotate_vector.hpp"

#include <GLUT/glut.h>

using namespace std;
using namespace glm;
using namespace oogl;

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
    
    // Create shaders
    auto texture_shader = std::shared_ptr<GLShader>(new GLShader("shaders/texture.vert",  "shaders/texture.frag"));
    auto phong_shader = std::shared_ptr<GLShader>(new GLShader("shaders/phong.vert",  "shaders/phong.frag"));
    auto grass_shader = std::shared_ptr<GLShader>(new GLShader("shaders/grass.vert",  "shaders/phong.frag", "shaders/grass.geom"));
    auto fastphong_shader = std::shared_ptr<GLShader>(new GLShader("shaders/fastphong.vert",  "shaders/phong.frag", "shaders/fastphong.geom"));
    
    // Create objects
    create_cube(texture_shader);
    create_spider(texture_shader);
    create_terrain(phong_shader);
    create_grass(grass_shader);
    
    grass->set_uniform_variable("lightPos", light_pos);
    terrain->set_uniform_variable("lightPos", light_pos);
    
    glutMainLoop();
}

void View::display()
{
    if (glutGet(GLUT_WINDOW_WIDTH) != WIN_SIZE_X || glutGet(GLUT_WINDOW_HEIGHT) != WIN_SIZE_Y) {
        return;
    }
    GLWrapper::initialize_draw();
    
    GLWrapper::cull_backface();
    spider->draw();
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

mat4 compute_spider_model_matrix(vec3 spider_position, vec3 spider_view_direction)
{
    mat4 spider_scale = scale(mat4(), vec3(10.f));
    mat4 spider_rotation = orientation(vec3(spider_view_direction.x, 0.f, spider_view_direction.z), vec3(0., 0., 1.));
    mat4 spider_translation = translate(mat4(), spider_position);
    return spider_translation * spider_rotation * spider_scale;
}

void View::animate()
{
    GLfloat timeValue = glutGet(GLUT_ELAPSED_TIME)*0.002;
    glm::vec3 animation(0.5 * sin(timeValue) + 0.5, 0., 0.);
    cube->set_model_matrix(translate(mat4(), animation));
    
    vec3 spider_position = model->get_spider_position();
    vec3 spider_view_direction = model->get_spider_view_direction();
    spider->set_model_matrix(compute_spider_model_matrix(spider_position, spider_view_direction));
    
    if(view_type == FIRST_PERSON)
    {
        GLWrapper::set_view(spider_position - 0.5f * spider_view_direction + vec3(0.,0.4,0.), spider_view_direction);
    }
    else if(view_type == THIRD_PERSON)
    {
        vec3 camera_view = normalize(vec3(0., -0.5, 0.) + spider_view_direction);
        GLWrapper::set_view(spider_position - 3.f * camera_view, camera_view);
    }
    
    grass->set_uniform_variable("spiderPosition", spider_position);
    grass->set_uniform_variable("wind", animation);
    
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
        case 'v':
            if(view_type == FIRST_PERSON)
            {
                view_type = THIRD_PERSON;
            }
            else if(view_type == THIRD_PERSON)
            {
                view_type = FIRST_PERSON;
            }
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

void View::create_grass(shared_ptr<GLShader> shader)
{
    auto material = GLMaterial {{0.2f,0.2f,0.f, 1.f}, {0.2f, 0.4f, 0.f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
    grass = std::unique_ptr<GLObject>(new GLObject(shader, material, GL_LINES));
    grass->initialize_vertex_attributes({"end_point"}, {3});
}

void View::create_terrain(shared_ptr<GLShader> shader)
{
    auto material = GLMaterial {{0.25f,0.25f,0.25f, 1.f}, {0.4f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
    terrain = std::unique_ptr<GLObject>(new GLObject(shader, material, GL_TRIANGLE_STRIP));
    terrain->initialize_vertex_attributes({"position", "normal"}, {3, 3});
}

void View::create_spider(shared_ptr<GLShader> shader)
{
    std::vector<glm::vec3> spider_vertices;
    std::vector<glm::vec2> spider_uvs;
    bool load_success = OBJLoader::load("models/spider/TRANTULA.OBJ", spider_vertices, spider_uvs);
    if(load_success)
    {
        tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile("models/spider/TRANTULA.PNG");
        bmp.flipVertically();
        auto texture = std::shared_ptr<GLTexture>(new GLTexture(shader, bmp, spider_uvs));
        
        auto material = GLMaterial {{0.5f,0.2f,0.f, 1.f}, {0.2f, 0.4f, 0.f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
        spider = std::unique_ptr<GLObject>(new GLObject(shader, material, GL_TRIANGLES, texture));
        
        spider->initialize_vertex_attributes({"position"}, {3});
        spider->set_vertex_attribute("position", spider_vertices);
        spider->finalize_vertex_attributes();
    }
}

void View::create_cube(shared_ptr<GLShader> shader)
{
    static const vector<vec3> cube_data = {
        // front
        vec3(-1.0, -1.0,  1.0),
        vec3(1.0, -1.0,  1.0),
        vec3(1.0,  1.0,  1.0),
        vec3(1.0,  1.0,  1.0),
        vec3(-1.0,  1.0,  1.0),
        vec3(-1.0, -1.0,  1.0),
        // top
        vec3(-1.0,  1.0,  1.0),
        vec3(1.0,  1.0,  1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(-1.0,  1.0, -1.0),
        vec3(-1.0,  1.0,  1.0),
        // back
        vec3(1.0, -1.0, -1.0),
        vec3(-1.0, -1.0, -1.0),
        vec3(-1.0,  1.0, -1.0),
        vec3(-1.0,  1.0, -1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(1.0, -1.0, -1.0),
        // bottom
        vec3(-1.0, -1.0, -1.0),
        vec3(1.0, -1.0, -1.0),
        vec3(1.0, -1.0,  1.0),
        vec3(1.0, -1.0,  1.0),
        vec3(-1.0, -1.0,  1.0),
        vec3(-1.0, -1.0, -1.0),
        // left
        vec3(-1.0, -1.0, -1.0),
        vec3(-1.0, -1.0,  1.0),
        vec3(-1.0,  1.0,  1.0),
        vec3(-1.0,  1.0,  1.0),
        vec3(-1.0,  1.0, -1.0),
        vec3(-1.0, -1.0, -1.0),
        // right
        vec3(1.0, -1.0,  1.0),
        vec3(1.0, -1.0, -1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(1.0,  1.0,  1.0),
        vec3(1.0, -1.0,  1.0)
    };
    
    auto material = GLMaterial {{0.15f,0.15f,0.15f, 1.f}, {0.4f, 0.2f, 0.6f, 1.f}, {0.2f, 0.2f, 0.8f, 1.f}};
    tdogl::Bitmap cubeTextureBmp = tdogl::Bitmap::bitmapFromFile("models/test_texture.jpg");
    cubeTextureBmp.flipVertically();
    vector<vec2> cube_uvs;
    for (int i = 0; i < 6; i++){
        cube_uvs.push_back(vec2(0.));
        cube_uvs.push_back(vec2(1., 0.));
        cube_uvs.push_back(vec2(1., 1.));
        cube_uvs.push_back(vec2(1., 1.));
        cube_uvs.push_back(vec2(0., 1.));
        cube_uvs.push_back(vec2(0.));
    }
    auto cubeTexture = shared_ptr<GLTexture>(new GLTexture(shader, cubeTextureBmp, cube_uvs));
    cube = unique_ptr<GLObject>(new GLObject(shader, material, GL_TRIANGLES, cubeTexture));
    
    cube->initialize_vertex_attributes({"position"}, {3});
    cube->set_vertex_attribute("position", cube_data);
    cube->finalize_vertex_attributes();
}

