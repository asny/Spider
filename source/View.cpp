//
//  controller.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "View.h"
#include "Reader.hpp"
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
    
    camera = std::unique_ptr<GLCamera>(new GLCamera());
    
    // Create shaders
    auto texture_shader = std::shared_ptr<GLShader>(new GLShader({{"position", 3}, {"uv_coordinates", 2}}, "shaders/texture.vert",  "shaders/texture.frag"));
    auto skybox_shader = std::shared_ptr<GLShader>(new GLShader({{"position", 3}}, "shaders/skybox.vert",  "shaders/skybox.frag"));
    auto phong_shader = std::shared_ptr<GLShader>(new GLShader({{"position", 3}, {"normal", 3}}, "shaders/phong.vert",  "shaders/phong.frag"));
    auto grass_shader = std::shared_ptr<GLShader>(new GLShader({{"end_point", 3}}, "shaders/grass.vert",  "shaders/phong.frag", "shaders/grass.geom"));
    auto fastphong_shader = std::shared_ptr<GLShader>(new GLShader({{"position", 3}}, "shaders/fastphong.vert",  "shaders/phong.frag", "shaders/fastphong.geom"));
    
    // Create objects
    create_cube(texture_shader);
    create_skybox(skybox_shader);
    create_spider(fastphong_shader);
    create_terrain(phong_shader);
    create_grass(grass_shader);
    
    terrain_patches.front()->update_uniform_variable("lightPos", light_pos);
    grass_patches.front()->update_uniform_variable("lightPos", light_pos);
    
    glutMainLoop();
}

void print_fps()
{
    static int draws = 0;
    draws++;
    static float startTime = glutGet(GLUT_ELAPSED_TIME);
    float elapsedTime = 0.001 * (glutGet(GLUT_ELAPSED_TIME) - startTime);
    if(elapsedTime > 5)
    {
        std::cout << "FPS: " << (float)(draws)/elapsedTime << std::endl;
        startTime = glutGet(GLUT_ELAPSED_TIME);
        draws = 0;
    }
}

void View::display()
{
    if (glutGet(GLUT_WINDOW_WIDTH) != WIN_SIZE_X || glutGet(GLUT_WINDOW_HEIGHT) != WIN_SIZE_Y) {
        return;
    }
    
    vector<shared_ptr<GLObject>> objects = {spider, cube, skybox};
    objects.insert(objects.end(), terrain_patches.begin(), terrain_patches.end());
    objects.insert(objects.end(), grass_patches.begin(), grass_patches.end());
    
    camera->draw(objects);
    
    print_fps();
    
    glutSwapBuffers();
}

void View::reshape(int width, int height)
{
    WIN_SIZE_X = width;
    WIN_SIZE_Y = height;
    camera->set_screen_size(width, height);
    
    glutPostRedisplay();
}

mat4 compute_spider_model_matrix(vec3 spider_position, vec3 spider_view_direction)
{
    mat4 spider_scale = scale(mat4(), vec3(10.));
    mat4 spider_rotation_yaw = orientation(normalize(vec3(spider_view_direction.x, 0., spider_view_direction.z)), vec3(0., 0., 1.));
    mat4 spider_rotation_pitch = orientation(normalize(vec3(0., spider_view_direction.y, 1.)), vec3(0., 0., 1.));
    mat4 spider_translation = translate(mat4(), spider_position);
    return spider_translation * spider_rotation_yaw * spider_rotation_pitch * spider_scale;
}

void View::animate()
{
    double time = glutGet(GLUT_ELAPSED_TIME)*0.002;
    glm::vec3 animation(0.5 * sin(time) + 0.5, 0., 0.5 * cos(time + 0.5) + 0.5);
    cube->set_model_matrix(translate(mat4(), animation));
    
    vec3 spider_position = model->get_spider_position();
    vec3 spider_view_direction = model->get_spider_view_direction();
    spider->set_model_matrix(compute_spider_model_matrix(spider_position, spider_view_direction));
    
    switch (view_type) {
        case FIRST_PERSON:
            camera->set_view(spider_position - 0.5f * spider_view_direction + vec3(0.,0.4,0.), spider_view_direction);
            break;
        case THIRD_PERSON:
        {
            vec3 camera_view = normalize(vec3(0., -0.5, 0.) + spider_view_direction);
            camera->set_view(spider_position - 2.f * camera_view, camera_view);
        }
            break;
        case BIRD:
        {
            vec3 camera_view = normalize(vec3(0., -1., 0.) + 0.1f * vec3(spider_view_direction.x, 0., spider_view_direction.z));
            camera->set_view(spider_position - 4.f * camera_view, camera_view);
        }
            break;
    }
    
    grass_patches.front()->update_uniform_variable("spiderPosition", spider_position);
    grass_patches.front()->update_uniform_variable("wind", animation);
    
    for (auto patch_index : model->terrain_patches_to_update())
    {
        std::vector<glm::vec3> terrain_positions, terrain_normals, grass_end_points;
        model->get_terrain_patch(patch_index.second, terrain_positions, terrain_normals, grass_end_points);
        
        terrain_patches[patch_index.first]->update_vertex_attribute("position", terrain_positions);
        terrain_patches[patch_index.first]->update_vertex_attribute("normal", terrain_normals);
        terrain_patches[patch_index.first]->finalize_vertex_attributes();
        
        grass_patches[patch_index.first]->update_vertex_attribute("end_point", grass_end_points);
        grass_patches[patch_index.first]->finalize_vertex_attributes();
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
            model->jump();
            break;
        case 'v':
            if(view_type == FIRST_PERSON)
            {
                view_type = THIRD_PERSON;
            }
            else if(view_type == THIRD_PERSON)
            {
                view_type = BIRD;
            }
            else if(view_type == BIRD)
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
    auto material = GLMaterial {{0.1f,0.3f,0.f, 1.f}, {0.2f, 0.4f, 0.f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
    for (int i = 0; i < 9; i++) {
        grass_patches.push_back(shared_ptr<GLObject>(new GLObject(shader, material, GL_LINES, nullptr, false)));
    }
}

void View::create_terrain(shared_ptr<GLShader> shader)
{
    auto material = GLMaterial {{0.25f,0.25f,0.25f, 1.f}, {0.4f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
    for (int i = 0; i < 9; i++) {
        terrain_patches.push_back(shared_ptr<GLObject>(new GLObject(shader, material, GL_TRIANGLE_STRIP)));
    }
}

void View::create_spider(shared_ptr<GLShader> shader)
{
    std::vector<glm::vec3> spider_vertices;
    std::vector<glm::vec2> spider_uvs;
    bool load_success = Reader::load_obj("resources/spider/TRANTULA.OBJ", spider_vertices, spider_uvs);
    if(load_success)
    {
        auto material = GLMaterial {{0.1f,0.1f,0.1f, 1.f}, {0.3f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
        spider = shared_ptr<GLObject>(new GLObject(shader, material, GL_TRIANGLES));
        
        spider->update_vertex_attribute("position", spider_vertices);
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
    auto cubeTextureBmp = Reader::load_bitmap("resources/test_texture.jpg");
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
    auto cubeTexture = shared_ptr<GLTexture>(new GLTexture2D(cubeTextureBmp));
    cube = shared_ptr<GLObject>(new GLObject(shader, material, GL_TRIANGLES, cubeTexture));
    
    cube->update_vertex_attribute("position", cube_data);
    cube->update_vertex_attribute("uv_coordinates", cube_uvs);
    cube->finalize_vertex_attributes();
}

void View::create_skybox(shared_ptr<GLShader> shader)
{
    static const vector<vec3> cube_data = {
        // front
        vec3(-1.0, -1.0,  1.0),
        vec3(1.0,  1.0,  1.0),
        vec3(1.0, -1.0,  1.0),
        vec3(1.0,  1.0,  1.0),
        vec3(-1.0, -1.0,  1.0),
        vec3(-1.0,  1.0,  1.0),
        // top
        vec3(-1.0,  1.0,  1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(1.0,  1.0,  1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(-1.0,  1.0,  1.0),
        vec3(-1.0,  1.0, -1.0),
        // back
        vec3(1.0, -1.0, -1.0),
        vec3(-1.0,  1.0, -1.0),
        vec3(-1.0, -1.0, -1.0),
        vec3(-1.0,  1.0, -1.0),
        vec3(1.0, -1.0, -1.0),
        vec3(1.0,  1.0, -1.0),
        // bottom
        vec3(-1.0, -1.0, -1.0),
        vec3(1.0, -1.0,  1.0),
        vec3(1.0, -1.0, -1.0),
        vec3(1.0, -1.0,  1.0),
        vec3(-1.0, -1.0, -1.0),
        vec3(-1.0, -1.0,  1.0),
        // left
        vec3(-1.0, -1.0, -1.0),
        vec3(-1.0,  1.0,  1.0),
        vec3(-1.0, -1.0,  1.0),
        vec3(-1.0,  1.0,  1.0),
        vec3(-1.0, -1.0, -1.0),
        vec3(-1.0,  1.0, -1.0),
        // right
        vec3(1.0, -1.0,  1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(1.0, -1.0, -1.0),
        vec3(1.0,  1.0, -1.0),
        vec3(1.0, -1.0,  1.0),
        vec3(1.0,  1.0,  1.0)
    };
    
    auto material = GLMaterial {{0.15f,0.15f,0.15f, 1.f}, {0.4f, 0.2f, 0.6f, 1.f}, {0.2f, 0.2f, 0.8f, 1.f}};
    const string path = "resources/skybox_evening/";
    
    auto bitmaps = {Reader::load_bitmap(path + "right.jpg"), Reader::load_bitmap(path + "left.jpg"), Reader::load_bitmap(path + "top.jpg"), Reader::load_bitmap(path + "top.jpg"), Reader::load_bitmap(path + "front.jpg"), Reader::load_bitmap(path + "back.jpg")};
    auto skybox_texture = shared_ptr<GLTexture>(new GLTexture3D(bitmaps));
    skybox = shared_ptr<GLObject>(new GLObject(shader, material, GL_TRIANGLES, skybox_texture));
    
    skybox->update_vertex_attribute("position", cube_data);
    skybox->finalize_vertex_attributes();
}

