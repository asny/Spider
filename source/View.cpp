//
//  controller.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "View.h"
#include "gtx/rotate_vector.hpp"
#include "MeshCreator.h"
#include "Materials.h"
#include "Random.h"
#include "effects/GLFogEffect.h"

#define GLFW_INCLUDE_NONE
#include "glfw3.h"

using namespace std;
using namespace glm;
using namespace gle;
using namespace mesh;

View* View::instance = NULL;
GLFWwindow* gWindow = NULL;

void OnError(int errorCode, const char* msg)
{
    throw std::runtime_error(msg);
}

View::View(int &argc, char** argv)
{
    int WIN_SIZE_X = 1400;
    int WIN_SIZE_Y = 700;
    
    instance = this;
    
    // initialise GLFW
    glfwSetErrorCallback(OnError);
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");
    
    // Open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    gWindow = glfwCreateWindow(WIN_SIZE_X, WIN_SIZE_Y, "Spider game", NULL, NULL);
    if(!gWindow)
        throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");
    
    // GLFW settings
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, 0, 0);
    glfwMakeContextCurrent(gWindow);
    
    glfwGetFramebufferSize(gWindow, &WIN_SIZE_X, &WIN_SIZE_Y);
    
    // Create camera
    camera = std::unique_ptr<GLCamera>(new GLCamera(WIN_SIZE_X, WIN_SIZE_Y));
    auto fog_effect = std::make_shared<GLFogEffect>(make_shared<GLTexture2D>("resources/water_noise.jpg"));
    camera->set_post_effect(fog_effect);
    
    // Create scene
    scene = unique_ptr<GLScene>(new GLScene());
    
    // Create model
    model = std::unique_ptr<Model>(new Model());
    
    // Create objects
    create_skybox();
    create_cube();
    create_terrain();
//    create_grass();
    create_spider_body();
    create_spider_legs();
//    create_fog();
    
    // Create light
    scene->add_light(std::make_shared<GLDirectionalLight>(normalize(vec3(-0.5, -0.5, 0.))));
    
    // run while the window is open
    double lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(gWindow))
    {
        // process pending events
        glfwPollEvents();
        
        // update the scene based on the time elapsed since last update
        *time = glfwGetTime();
        fog_effect->time = *time;
        
        update(*time - lastTime);
        Butterfly::spawn_and_destroy_and_update(*scene, *time);
        
        *wind = glm::vec3(0.5 * sin(*time) + 0.5, 0., 0.5 * cos(*time + 0.5) + 0.5);
        
        lastTime = *time;
        
        // draw one frame
        camera->draw(*scene);
        
        glfwSwapBuffers(gWindow);
        
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
    else if(glfwGetKey(gWindow, 'N'))
    {
        camera->wireframe(true);
    }
    else if(glfwGetKey(gWindow, 'M'))
    {
        camera->wireframe(false);
    }
    
    model->update(elapsedTime);
    *instance->spider_pos = instance->model->get_spider()->get_position();
    
    if(glfwGetKey(gWindow, '1'))
    {
        view_type = FIRST_PERSON;
    }
    else if(glfwGetKey(gWindow, '2'))
    {
        view_type = THIRD_PERSON;
    }
    else if(glfwGetKey(gWindow, '3'))
    {
        view_type = BIRD;
    }
    else if(glfwGetKey(gWindow, '4'))
    {
        view_type = WORM;
    }
    update_camera();
}

void View::update_camera()
{
    vec3 spider_position = instance->model->get_spider()->get_position();
    vec3 spider_view_direction = instance->model->get_spider()->get_view_direction();
    
    vec3 bird_view = normalize(vec3(0., -1., 0.) + 0.1f * vec3(spider_view_direction.x, 0., spider_view_direction.z));
    vec3 third_person_view = normalize(vec3(spider_view_direction.x, -0.5, spider_view_direction.z));
    vec3 worm_view = normalize(vec3(0., 1., 0.) + 0.1f * vec3(spider_view_direction.x, 0., spider_view_direction.z));
    
    switch (instance->view_type) {
        case FIRST_PERSON:
            instance->camera->set_view(spider_position - 0.5f * spider_view_direction + vec3(0.,0.4,0.), spider_view_direction);
            break;
        case THIRD_PERSON:
            instance->camera->set_view(spider_position - 2.f * third_person_view, third_person_view);
            break;
        case BIRD:
            instance->camera->set_view(spider_position - 4.f * bird_view, bird_view);
            break;
        case WORM:
            instance->camera->set_view(spider_position - 4.f * worm_view, worm_view);
            break;
    }
}

void View::create_grass()
{
    auto material = make_shared<GLGrassMaterial>(spider_pos, wind, vec3(0.3f,0.7f,0.f));
    for (TerrainPatch& patch : model->get_terrain_patches())
    {
        instance->scene->add_leaf(patch.get_grass(), material);
    }
}

void View::create_terrain()
{
    auto ground_texture = make_shared<GLTexture2D>("resources/grass.jpg");
    auto lake_texture = make_shared<GLTexture2D>("resources/bottom.png");
    auto noise_texture = make_shared<GLTexture2D>("resources/water_noise.jpg");
    
    for (TerrainPatch& patch : model->get_terrain_patches())
    {
        auto material = make_shared<TerrainMaterial>(time, ground_texture, lake_texture, noise_texture, patch.get_uv_coordinates());
        scene->add_leaf(patch.get_ground(), material);
        
        auto water_material = make_shared<WaterMaterial>(time, skybox_texture, noise_texture, patch.get_water_uv_coordinates());
        instance->scene->add_leaf(patch.get_water(), water_material);
    }
}

void View::create_spider_body()
{
    auto spider_transformation = std::make_shared<GLTransformationNode>(model->get_spider()->get_local2world());
    scene->add_child(spider_transformation);
    
    auto geometry = shared_ptr<Mesh>(new Mesh());
    auto normals = shared_ptr<Attribute<VertexID, vec3>>(new Attribute<VertexID, vec3>());
    MeshCreator::load_from_obj("resources/spider/spider.obj", *geometry, *normals);
    
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
    auto material = make_shared<GLColorMaterial>(vec3(0.3f, 0.2f, 0.2f), normals);
    spider_transformation->add_leaf(geometry, material);
}

void View::create_spider_legs()
{
    auto material = make_shared<GLSpiderLegsMaterial>(glm::vec3(0.3f, 0.2f, 0.2f));
    auto geometry = model->get_spider()->get_legs();
    scene->add_leaf(geometry, material);
}

void View::create_cube()
{
    auto geometry = MeshCreator::create_box(false);
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
    
    auto test_texture = make_shared<GLTexture2D>("resources/test_texture.jpg");
    auto material = shared_ptr<GLMaterial>(new GLTextureMaterial(test_texture, uv_attribute));
    scene->add_leaf(geometry, material);
}

void View::create_skybox()
{
    const string path = "resources/skybox_evening/";
    auto filenames = {path + "right.jpg", path + "left.jpg", path + "top.jpg", path + "top.jpg", path + "front.jpg", path + "back.jpg"};
    skybox_texture = make_shared<GLTexture3D>(filenames);
    auto material = make_shared<GLSkyboxMaterial>(skybox_texture);
    auto geometry = MeshCreator::create_box(true);
    scene->add_leaf(geometry, material);
}

void View::create_fog()
{
    std::shared_ptr<mesh::Mesh> geometry = std::make_shared<mesh::Mesh>();
    std::shared_ptr<mesh::Attribute<mesh::VertexID, glm::vec3>> normals = std::make_shared<mesh::Attribute<mesh::VertexID, glm::vec3>>();
    for(int i = 0; i < 2000; i++)
    {
        float radius = Random::value(3., 10.);
        double theta = Random::value(0., 2. * M_PI);
        double phi = Random::value(0.3 * M_PI, 0.7 * M_PI);
        glm::vec3 direction = glm::vec3(cos(theta) * sin(phi), cos(phi), sin(theta) * sin(phi));
        
        glm::vec3 tangent = cross(direction, glm::vec3(0., 1., 0.));
        glm::vec3 normal = cross(direction, tangent);
        double alpha = Random::value(0., 0.5 * M_PI) - 0.25 * M_PI;
        float sign = rand()%2 == 0 ? -1.f : 1.f;
        normal = sign * normalize(cosf(alpha) * normal + sinf(alpha) * tangent);
        
        auto vertexId = geometry->create_vertex(radius * direction);
        normals->at(vertexId) = normal;
    }
    
    auto material = make_shared<GLFogMaterial>(normals, time, 1.);
    scene->add_leaf(geometry, material);
}

