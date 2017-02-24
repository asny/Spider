//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include <iostream>
#include "gtx/rotate_vector.hpp"

#include "model.hpp"
#include "GLCamera.h"
#include "Butterfly.hpp"
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

GLFWwindow* gWindow = NULL;
std::unique_ptr<Model> model;

enum VIEW_TYPE { FIRST_PERSON, THIRD_PERSON, BIRD, WORM };

VIEW_TYPE view_type = FIRST_PERSON;

std::unique_ptr<gle::GLCamera> camera;
std::unique_ptr<gle::GLScene> scene;

std::shared_ptr<glm::vec3> wind = std::make_shared<glm::vec3>(0., 0., 0.);
std::shared_ptr<glm::vec3> spider_pos = std::make_shared<glm::vec3>(0., 0., 0.);
std::shared_ptr<float> t = std::make_shared<float>(0.f);

std::shared_ptr<gle::GLTexture3D> skybox_texture;

void OnError(int errorCode, const char* msg)
{
    throw std::runtime_error(msg);
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

void update_camera()
{
    vec3 spider_position = model->get_spider()->get_position();
    vec3 spider_view_direction = model->get_spider()->get_view_direction();
    
    vec3 bird_view = normalize(vec3(0., -1., 0.) + 0.1f * vec3(spider_view_direction.x, 0., spider_view_direction.z));
    vec3 third_person_view = normalize(vec3(spider_view_direction.x, -0.5, spider_view_direction.z));
    vec3 worm_view = normalize(vec3(0., 1., 0.) + 0.1f * vec3(spider_view_direction.x, 0., spider_view_direction.z));
    
    switch (view_type) {
        case FIRST_PERSON:
            camera->set_view(spider_position - 0.5f * spider_view_direction + vec3(0.,0.4,0.), spider_view_direction);
            break;
        case THIRD_PERSON:
            camera->set_view(spider_position - 2.f * third_person_view, third_person_view);
            break;
        case BIRD:
            camera->set_view(spider_position - 4.f * bird_view, bird_view);
            break;
        case WORM:
            camera->set_view(spider_position - 4.f * worm_view, worm_view);
            break;
    }
}

void update(double elapsedTime)
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
    *spider_pos = model->get_spider()->get_position();
    
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

void create_grass()
{
    auto material = make_shared<GLGrassMaterial>(spider_pos, wind, vec3(0.3f,0.7f,0.f));
    for (TerrainPatch& patch : model->get_terrain_patches())
    {
        scene->add_leaf(patch.get_grass(), material);
    }
}

void create_cube()
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

void create_skybox()
{
    const string path = "resources/skybox_evening/";
    auto filenames = {path + "right.jpg", path + "left.jpg", path + "top.jpg", path + "top.jpg", path + "front.jpg", path + "back.jpg"};
    skybox_texture = make_shared<GLTexture3D>(filenames);
    auto material = make_shared<GLSkyboxMaterial>(skybox_texture);
    auto geometry = MeshCreator::create_box(true);
    scene->add_leaf(geometry, material);
}

int main(int argc, char** argv)
{
    int WIN_SIZE_X = 1400;
    int WIN_SIZE_Y = 700;
    
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
    model = std::unique_ptr<Model>(new Model(*scene));
    
    // Create objects
    create_skybox();
    create_cube();
    //    create_grass();
    
    // Create light
    scene->add_light(std::make_shared<GLDirectionalLight>(normalize(vec3(-0.5, -0.5, 0.))));
    
    // run while the window is open
    double lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(gWindow))
    {
        // process pending events
        glfwPollEvents();
        
        // update the scene based on the time elapsed since last update
        *t = glfwGetTime();
        fog_effect->time = *t;
        
        update(*t - lastTime);
        Butterfly::spawn_and_destroy_and_update(*scene, *t);
        
        *wind = glm::vec3(0.5 * sin(*t) + 0.5, 0., 0.5 * cos(*t + 0.5) + 0.5);
        
        lastTime = *t;
        
        // draw one frame
        camera->draw(*scene);
        
        glfwSwapBuffers(gWindow);
        
        //exit program if escape key is pressed
        if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }
    
    // clean up and exit
    glfwTerminate();
    return 0;
}
