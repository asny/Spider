//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "gtx/rotate_vector.hpp"
#include "Random.h"

#include "GLCamera.h"
#include "MeshCreator.h"
#include "Materials.h"
#include "effects/GLFogEffect.h"

#include "Spider.hpp"
#include "Terrain.hpp"
#include "Butterfly.hpp"

#define GLFW_INCLUDE_NONE
#include "glfw3.h"

using namespace std;
using namespace glm;
using namespace gle;
using namespace mesh;
using namespace std::chrono;

GLFWwindow* gWindow = NULL;

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

void update_camera(GLCamera& camera, const glm::vec3& spider_position, const glm::vec3& spider_view_direction)
{
    enum VIEW_TYPE { FIRST_PERSON, THIRD_PERSON, BIRD, WORM };
    
    static VIEW_TYPE view_type = FIRST_PERSON;
    
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
    
    vec3 bird_view = normalize(vec3(0., -1., 0.) + 0.1f * vec3(spider_view_direction.x, 0., spider_view_direction.z));
    vec3 third_person_view = normalize(vec3(spider_view_direction.x, -0.5, spider_view_direction.z));
    vec3 worm_view = normalize(vec3(0., 1., 0.) + 0.1f * vec3(spider_view_direction.x, 0., spider_view_direction.z));
    
    switch (view_type) {
        case FIRST_PERSON:
            camera.set_view(spider_position - 0.5f * spider_view_direction + vec3(0.,0.4,0.), spider_view_direction);
            break;
        case THIRD_PERSON:
            camera.set_view(spider_position - 2.f * third_person_view, third_person_view);
            break;
        case BIRD:
            camera.set_view(spider_position - 4.f * bird_view, bird_view);
            break;
        case WORM:
            camera.set_view(spider_position - 4.f * worm_view, worm_view);
            break;
    }
}

void update_spider(Spider& spider, double elapsedTime)
{
    if(glfwGetKey(gWindow, ' '))
    {
        spider.jump(glfwGetKey(gWindow, 'W'));
    }
    if(glfwGetKey(gWindow, 'S'))
    {
        spider.move(-elapsedTime);
    }
    else if(glfwGetKey(gWindow, 'W'))
    {
        spider.move(elapsedTime);
    }
    if(glfwGetKey(gWindow, 'A'))
    {
        spider.rotate(elapsedTime);
    }
    else if(glfwGetKey(gWindow, 'D'))
    {
        spider.rotate(-elapsedTime);
    }
    
    spider.update(elapsedTime);
}

void create_cube(GLNode& root)
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
    root.add_leaf(geometry, material);
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
    auto camera = GLCamera(WIN_SIZE_X, WIN_SIZE_Y);
    auto fog_effect = std::make_shared<GLFogEffect>(make_shared<GLTexture2D>("resources/water_noise.jpg"));
    camera.set_post_effect(fog_effect);
    
    // Create scene
    auto scene = GLScene();
    
    // Create objects
    auto initial_position = glm::vec3(0., 0.3, -5.);
    auto terrain = Terrain(scene, initial_position);
    
    std::function<double(glm::vec3)> get_height_at = std::bind(&Terrain::get_height_at, &terrain, std::placeholders::_1);
    auto spider = Spider(scene, initial_position, glm::vec3(0., 0., 1.), get_height_at);
    
    create_cube(scene);
    
    // Create light
    scene.add_light(std::make_shared<GLDirectionalLight>(normalize(vec3(-0.5, -0.5, 0.))));
    
    // run while the window is open
    auto start_time = high_resolution_clock::now();
    auto last_time = start_time;
    while(!glfwWindowShouldClose(gWindow))
    {
        // process pending events
        glfwPollEvents();
        
        // update the scene based on the time elapsed since last update
        auto current_time = high_resolution_clock::now();
        double time = 0.001 * duration_cast<milliseconds>(current_time - start_time).count();
        double elapsed_time = 0.001 * duration_cast<milliseconds>(current_time - last_time).count();
        last_time = current_time;
        
        print_fps(elapsed_time);
        update_spider(spider, elapsed_time);
        update_camera(camera, spider.get_position(), spider.get_view_direction());
        
        fog_effect->time = time;
        Butterfly::spawn_and_destroy_and_update(scene, time);
        terrain.update(time, spider.get_position());
        
        // draw one frame
        camera.draw(scene);
        
        glfwSwapBuffers(gWindow);
        
        //exit program if escape key is pressed
        if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }
    
    // clean up and exit
    glfwTerminate();
    return 0;
}
