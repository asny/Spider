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

#define GLFW_INCLUDE_NONE
#include <glfw3.h>

using namespace std;
using namespace glm;
using namespace oogl;

View* View::instance = NULL;
GLFWwindow* gWindow = NULL;

void OnError(int errorCode, const char* msg) {
    throw std::runtime_error(msg);
}

View::View(int &argc, char** argv)
{
    const int WIN_SIZE_X = 1200;
    const int WIN_SIZE_Y = 700;
    
    instance = this;
    
    // initialise GLFW
    glfwSetErrorCallback(OnError);
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");
    
    // Open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    gWindow = glfwCreateWindow(WIN_SIZE_X, WIN_SIZE_Y, "Spider game", NULL, NULL);
    if(!gWindow)
        throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");
    
    // GLFW settings
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, 0, 0);
    glfwMakeContextCurrent(gWindow);
    
    // Create camera
    camera = std::unique_ptr<GLCamera>(new GLCamera());
    camera->set_screen_size(WIN_SIZE_X, WIN_SIZE_Y);
    
    // Create shaders
    auto texture_shader = std::shared_ptr<GLShader>(new GLShader("shaders/texture.vert",  "shaders/texture.frag"));
    auto skybox_shader = std::shared_ptr<GLShader>(new GLShader("shaders/skybox.vert",  "shaders/skybox.frag"));
    auto phong_shader = std::shared_ptr<GLShader>(new GLShader("shaders/phong.vert",  "shaders/phong.frag"));
    auto grass_shader = std::shared_ptr<GLShader>(new GLShader("shaders/pre_geom.vert",  "shaders/grass.frag", "shaders/grass.geom"));
    auto spider_legs_shader = std::shared_ptr<GLShader>(new GLShader("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/spider_legs.geom"));
    auto fastphong_shader = std::shared_ptr<GLShader>(new GLShader("shaders/pre_geom.vert",  "shaders/phong.frag", "shaders/fastphong.geom"));
    
    // Create objects
    create_cube(texture_shader);
    create_skybox(skybox_shader);
    create_spider_body(phong_shader);
    create_spider_legs(spider_legs_shader);
    create_terrain(phong_shader);
    create_grass(grass_shader);
    
    spider_body->update_uniform_variable("lightPos", light_pos);
    spider_legs->update_uniform_variable("lightPos", light_pos);
    terrain_patches.front()->update_uniform_variable("lightPos", light_pos);
    grass_patches.front()->update_uniform_variable("lightPos", light_pos);
    
    // Create model
    model = std::unique_ptr<Model>(new Model(
    [](){
        update_spider();
        update_terrain_and_grass();
        update_camera();
    },
    [](){
        update_spider();
        update_camera();
    }));
    
    // Update
    update_terrain_and_grass();
    update_spider();
    update_camera();
    
    // run while the window is open
    double lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(gWindow))
    {
        // process pending events
        glfwPollEvents();
        
        // update the scene based on the time elapsed since last update
        double thisTime = glfwGetTime();
        
        update(thisTime - lastTime);
        
        glm::vec3 wind(0.5 * sin(thisTime) + 0.5, 0., 0.5 * cos(thisTime + 0.5) + 0.5);
        grass_patches.front()->update_uniform_variable("wind", wind);
        
        lastTime = thisTime;
        
        // draw one frame
        display();
        
        //exit program if escape key is pressed
        if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }
    
    // clean up and exit
    glfwTerminate();
}

void View::display()
{
    vector<shared_ptr<GLObject>> objects = {spider_body, spider_legs, cube, skybox};
    objects.insert(objects.end(), terrain_patches.begin(), terrain_patches.end());
    if(display_grass)
    {
        objects.insert(objects.end(), grass_patches.begin(), grass_patches.end());
    }
    
    camera->draw(objects);
    
    glfwSwapBuffers(gWindow);
}

void print_fps(double elapsedTime)
{
    static int draws = 0;
    draws++;
    static float seconds = 0.;
    seconds += elapsedTime;
    if(seconds > 5)
    {
        std::cout << "FPS: " << ((float)draws)/seconds << std::endl;
        seconds = 0.;
        draws = 0;
    }
}

void View::update(double elapsedTime)
{
    print_fps(elapsedTime);
    
    if(glfwGetKey(gWindow, ' '))
    {
        model->jump(glfwGetKey(gWindow, 'W'));
    }
    model->update_jump(elapsedTime);
    
    if(glfwGetKey(gWindow, 'S'))
    {
        model->move(-elapsedTime);
    }
    else if(glfwGetKey(gWindow, 'W'))
    {
        model->move(elapsedTime);
    }
    if(glfwGetKey(gWindow, 'A'))
    {
        model->rotate(elapsedTime);
    }
    else if(glfwGetKey(gWindow, 'D'))
    {
        model->rotate(-elapsedTime);
    }
    
    if(glfwGetKey(gWindow, '1'))
    {
        view_type = FIRST_PERSON;
        update_camera();
    }
    else if(glfwGetKey(gWindow, '2'))
    {
        view_type = THIRD_PERSON;
        update_camera();
    }
    else if(glfwGetKey(gWindow, '3'))
    {
        view_type = BIRD;
        update_camera();
    }
    
    if(glfwGetKey(gWindow, 'G'))
    {
        display_grass = true;
    }
    else if(glfwGetKey(gWindow, 'F'))
    {
        display_grass = false;
    }
}

void View::update_camera()
{
    vec3 spider_position = instance->model->get_spider_position();
    vec3 spider_view_direction = instance->model->get_spider_view_direction();
    
    switch (instance->view_type) {
        case FIRST_PERSON:
            instance->camera->set_view(spider_position - 0.5f * spider_view_direction + vec3(0.,0.4,0.), spider_view_direction);
            break;
        case THIRD_PERSON:
        {
            vec3 camera_view = normalize(vec3(0., -0.5, 0.) + spider_view_direction);
            instance->camera->set_view(spider_position - 2.f * camera_view, camera_view);
        }
            break;
        case BIRD:
        {
            vec3 camera_view = normalize(vec3(0., -1., 0.) + 0.1f * vec3(spider_view_direction.x, 0., spider_view_direction.z));
            instance->camera->set_view(spider_position - 4.f * camera_view, camera_view);
        }
            break;
    }
}

void View::update_spider()
{
    vec3 spider_position = instance->model->get_spider_position();
    vec3 spider_view_direction = instance->model->get_spider_view_direction();
    
    // Compute spider model matrix
    mat4 spider_rotation_yaw = orientation(normalize(vec3(spider_view_direction.x, 0., spider_view_direction.z)), vec3(0., 0., 1.));
    mat4 spider_rotation_pitch = orientation(normalize(vec3(0., spider_view_direction.y, 1.)), vec3(0., 0., 1.));
    mat4 spider_translation = translate(mat4(), spider_position);
    mat4 model_matrix = spider_translation * spider_rotation_yaw * spider_rotation_pitch;
    
    // Update uniform variables
    instance->spider_body->set_model_matrix(model_matrix);
    
    // Update vertex attributes
    instance->spider_legs->update_vertex_attribute("position", instance->model->get_spider_feet_positions(model_matrix));
    instance->spider_legs->finalize_vertex_attributes();
}

void View::update_terrain_and_grass()
{
    instance->grass_patches.front()->update_uniform_variable("spiderPosition", instance->model->get_spider_position());
    for (auto patch_index : instance->model->terrain_patches_to_update())
    {
        vector<vec3> terrain_positions, terrain_normals, grass_end_points;
        instance->model->get_terrain_patch(patch_index, terrain_positions, terrain_normals, grass_end_points);
        
        instance->terrain_patches[patch_index]->update_vertex_attribute("position", terrain_positions);
        instance->terrain_patches[patch_index]->update_vertex_attribute("normal", terrain_normals);
        instance->terrain_patches[patch_index]->finalize_vertex_attributes();
        
        instance->grass_patches[patch_index]->update_vertex_attribute("position", grass_end_points);
        instance->grass_patches[patch_index]->finalize_vertex_attributes();
    }
}

void View::create_grass(shared_ptr<GLShader> shader)
{
    auto material = GLMaterial {{0.2f,0.5f,0.f, 1.f}, {0.1f, 0.2f, 0.f, 1.f}, {0.f, 0.f, 0.f, 0.f}};
    for (int i = 0; i < 9; i++) {
        grass_patches.push_back(shared_ptr<GLObject>(new GLObject({{"position", 3}}, shader, material, GL_LINES, nullptr, false)));
    }
}

void View::create_terrain(shared_ptr<GLShader> shader)
{
    auto material = GLMaterial {{0.25f,0.25f,0.25f, 1.f}, {0.4f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
    for (int i = 0; i < 9; i++) {
        terrain_patches.push_back(shared_ptr<GLObject>(new GLObject({{"position", 3}, {"normal", 3}}, shader, material, GL_TRIANGLE_STRIP)));
    }
}

void View::create_spider_body(shared_ptr<GLShader> shader)
{
    std::vector<glm::vec3> spider_vertices;
    std::vector<glm::vec3> spider_normals;
    bool load_success = Reader::load_obj("resources/spider/spider.obj", spider_vertices, spider_normals);
    if(load_success)
    {
        vec3 center;
        for (vec3 vertex : spider_vertices) {
            center += vertex;
        }
        center /= spider_vertices.size();
        
        for (vec3& vertex : spider_vertices) {
            vertex -= center;
        }
        
        auto material = GLMaterial {{0.1f,0.1f,0.1f, 1.f}, {0.3f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
        spider_body = shared_ptr<GLObject>(new GLObject({{"position", 3}, {"normal", 3}}, shader, material, GL_TRIANGLES));
        
        spider_body->update_vertex_attribute("position", spider_vertices);
        spider_body->update_vertex_attribute("normal", spider_normals);
        spider_body->finalize_vertex_attributes();
    }
}

void View::create_spider_legs(shared_ptr<GLShader> shader)
{
    auto material = GLMaterial {{0.1f,0.1f,0.1f, 1.f}, {0.3f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}};
    spider_legs = shared_ptr<GLObject>(new GLObject({{"position", 3}}, shader, material, GL_LINES));
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
    cube = shared_ptr<GLObject>(new GLObject({{"position", 3}, {"uv_coordinates", 2}}, shader, material, GL_TRIANGLES, cubeTexture));
    
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
    skybox = shared_ptr<GLObject>(new GLObject({{"position", 3}}, shader, material, GL_TRIANGLES, skybox_texture));
    
    skybox->update_vertex_attribute("position", cube_data);
    skybox->finalize_vertex_attributes();
}

