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
using namespace geogo;

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
    camera = std::shared_ptr<GLCamera>(new GLCamera());
    camera->set_screen_size(WIN_SIZE_X, WIN_SIZE_Y);
    
    // Create scene
    scene = unique_ptr<GLScene>(new GLScene(gWindow, camera));
    
    // Create model
    model = std::unique_ptr<Model>(new Model(
    [](){
        update_terrain_and_grass();
        update_camera();
    },
    [](){
        update_camera();
    }));
    
    // Create objects
    create_cube();
    create_skybox();
    create_spider_body();
    create_spider_legs();
    create_terrain();
    create_grass();
    
    // Update
    update_terrain_and_grass();
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
        
        *wind = glm::vec3(0.5 * sin(thisTime) + 0.5, 0., 0.5 * cos(thisTime + 0.5) + 0.5);
        
        lastTime = thisTime;
        
        // draw one frame
        scene->draw();
        
        //exit program if escape key is pressed
        if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }
    
    // clean up and exit
    glfwTerminate();
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
    model->update(elapsedTime);
    
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
}

void View::update_camera()
{
    vec3 spider_position = instance->model->get_spider()->get_position();
    vec3 spider_view_direction = instance->model->get_spider()->get_view_direction();
    
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

void View::update_terrain_and_grass()
{
    *instance->spider_pos = instance->model->get_spider()->get_position();
}

void View::create_grass()
{
    auto material = shared_ptr<GLMaterial>(new GLGrassMaterial({0.2f,0.5f,0.f, 1.f}, {0.1f, 0.2f, 0.f, 1.f}));
    for (TerrainPatch& patch : model->get_terrain_patches())
    {
        auto grass = shared_ptr<GLObject>(new GLObject(patch.get_grass(), material));
        grass->use_uniform("lightPos", light_pos);
        grass->use_uniform("spiderPosition", spider_pos);
        grass->use_uniform("wind", wind);
        instance->scene->add(grass);
    }
}

void View::create_terrain()
{
    auto material = shared_ptr<GLMaterial>(new GLStandardMaterial({0.25f,0.25f,0.25f, 1.f}, {0.4f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}));
    for (TerrainPatch& patch : model->get_terrain_patches())
    {
        auto ground = shared_ptr<GLObject>(new GLObject(patch.get_ground(), material));
        ground->use_attribute("normal", patch.get_ground_normals());
        ground->use_uniform("lightPos", light_pos);
        instance->scene->add(ground);
    }
}

void View::create_spider_body()
{
    auto geometry = shared_ptr<Geometry>(new Geometry());
    auto normal_attribute = shared_ptr<Attribute<VertexID, vec3>>(new Attribute<VertexID, vec3>());
    Reader::load_obj("resources/spider/spider.obj", *geometry, *normal_attribute);
    
    vec3 center;
    for(auto vertex = geometry->vertices_begin(); vertex != geometry->vertices_end(); vertex = vertex->next())
    {
        vec3 pos = geometry->position()->at(vertex);
        center += pos;
    }
    center /= geometry->get_no_vertices();
    
    for(auto vertex = geometry->vertices_begin(); vertex != geometry->vertices_end(); vertex = vertex->next())
    {
        vec3 p = geometry->position()->at(vertex);
        geometry->position()->at(vertex) = p - center + vec3(0,0,0.3);
    }
    auto material = shared_ptr<GLMaterial>(new GLStandardMaterial({0.1f,0.1f,0.1f, 1.f}, {0.3f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}));
    auto object = shared_ptr<GLObject>(new GLObject(geometry, material));
    object->use_attribute("normal", normal_attribute);
    object->use_uniform("lightPos", light_pos);
    object->set_model_matrix(model->get_spider()->get_local2world());
    scene->add(object);
}

void View::create_spider_legs()
{
    auto material = shared_ptr<GLMaterial>(new GLSpiderLegsMaterial({0.1f,0.1f,0.1f, 1.f}, {0.3f, 0.2f, 0.2f, 1.f}, {0.f, 0.f, 0.f, 1.f}));
    auto geometry = model->get_spider()->get_legs();
    auto object = shared_ptr<GLObject>(new GLObject(geometry, material));
    object->use_uniform("lightPos", light_pos);
    object->set_model_matrix(model->get_spider()->get_local2world());
    scene->add(object);
}

void View::create_cube()
{
    auto geometry = Geometry::create_box(false);
    auto uv_attribute = shared_ptr<Attribute<VertexID, vec2>>(new Attribute<VertexID, vec2>());
    
    for (auto vertex = geometry->vertices_begin(); vertex != geometry->vertices_end(); vertex = vertex->next())
    {
        vec3 pos = geometry->position()->at(vertex);
        auto uv = vec2(0., 0.);
        if(pos.x < 0)
            uv.x = 1.;
        if(pos.y > 0)
            uv.y = 1.;
        uv_attribute->at(*vertex) = uv;
    }
    
    auto cubeTextureBmp = Reader::load_bitmap("resources/test_texture.jpg");
    cubeTextureBmp.flipVertically();
    auto cubeTexture = shared_ptr<GLTexture>(new GLTexture2D(cubeTextureBmp));
    auto material = shared_ptr<GLMaterial>(new GLTextureMaterial(cubeTexture));
    auto object = shared_ptr<GLObject>(new GLObject(geometry, material));
    object->use_attribute("uv_coordinates", uv_attribute);
    scene->add(object);
}

void View::create_skybox()
{
    const string path = "resources/skybox_evening/";
    auto bitmaps = {Reader::load_bitmap(path + "right.jpg"), Reader::load_bitmap(path + "left.jpg"), Reader::load_bitmap(path + "top.jpg"), Reader::load_bitmap(path + "top.jpg"), Reader::load_bitmap(path + "front.jpg"), Reader::load_bitmap(path + "back.jpg")};
    auto skybox_texture = shared_ptr<GLTexture3D>(new GLTexture3D(bitmaps));
    auto material = shared_ptr<GLSkyboxMaterial>(new GLSkyboxMaterial(skybox_texture));
    auto geometry = Geometry::create_box(true);
    auto object = shared_ptr<GLObject>(new GLObject(geometry, material));
    scene->add(object);
}

