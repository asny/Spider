//
//  Created by Asger Nyman Christiansen on 03/10/15.
//  Copyright (c) 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "GLCamera.h"
#include "Materials.h"
#include "effects/GLDebugEffect.h"
#include "effects/GLFogEffect.h"
#include "effects/GLAmbientOcclusionEffect.h"

#include "Spider.hpp"
#include "Terrain.hpp"
#include "Butterfly.hpp"
#include "Firefly.hpp"

#define SDL_MAIN_HANDLED
#include "SDL.h"

using namespace std;
using namespace glm;
using namespace gle;
using namespace mesh;

enum VIEW_TYPE { FIRST_PERSON, THIRD_PERSON, BIRD, WORM };

VIEW_TYPE view_type = FIRST_PERSON;

bool ssao_enabled = false;

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

bool handle_events(Spider& spider, GLDebugEffect& debug_effect)
{
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 )
    {
        if( e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
        {
            return true;
        }
        switch( e.key.keysym.sym )
        {
            case SDLK_UP:
            case SDLK_w:
                spider.move_foward(e.type == SDL_KEYDOWN);
                break;
            case SDLK_DOWN:
            case SDLK_s:
                spider.move_backward(e.type == SDL_KEYDOWN);
                break;
            case SDLK_LEFT:
            case SDLK_a:
                spider.rotate_left(e.type == SDL_KEYDOWN);
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                spider.rotate_right(e.type == SDL_KEYDOWN);
                break;
            case SDLK_1:
                view_type = FIRST_PERSON;
                break;
            case SDLK_2:
                view_type = THIRD_PERSON;
                break;
            case SDLK_3:
                view_type = BIRD;
                break;
            case SDLK_4:
                view_type = WORM;
                break;
            case SDLK_5:
                debug_effect.type = gle::GLDebugEffect::DEPTH;
                break;
            case SDLK_6:
                debug_effect.type = gle::GLDebugEffect::POSITION;
                break;
            case SDLK_7:
                debug_effect.type = gle::GLDebugEffect::NORMAL;
                break;
            case SDLK_8:
                debug_effect.type = gle::GLDebugEffect::COLOR;
                break;
            case SDLK_9:
                debug_effect.type = gle::GLDebugEffect::NONE;
                break;
            case SDLK_0:
                if(e.type == SDL_KEYDOWN)
                    ssao_enabled = !ssao_enabled;
                break;
            case SDLK_SPACE:
                spider.jump();
                break;
            case SDLK_BACKSPACE:
                if(e.type == SDL_KEYDOWN)
                    GLShader::reload_shaders();
                break;
            default:
                break;
        }
    }
    return false;
}

int main(int argc, char** argv)
{
    // Initialize SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        throw std::runtime_error("SDL init failed");
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Create window
    int window_width = 1400;
    int window_height = 700;
    auto window = SDL_CreateWindow( "Spider game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE );
    if( window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        throw std::runtime_error("SDL init failed");
    }
    
    // Create context
    auto glcontext = SDL_GL_CreateContext(window);
    
    // Create camera
    auto camera = GLCamera(window_width, window_height);
    auto ssao_effect = GLAmbientOcclusionEffect();
    auto fog_effect = GLFogEffect(make_shared<GLTexture2D>("resources/water_noise.jpg"));
    auto debug_effect = GLDebugEffect();
    
    // Create scene
    auto scene = GLScene();
    
    // Create objects
    auto terrain = Terrain(scene);
    auto spider = Spider(scene, glm::vec3(0., 0., -5.), glm::vec3(0., 0., 1.));
    
    // Create light
    auto directional_light = make_shared<GLDirectionalLight>();
    scene.add_light(directional_light);
    directional_light->direction = normalize(vec3(-0.5, -0.5, 0.));
    
    bool quit = false;
    float last_time = gle::time();
    
    //While application is running
    while( !quit )
    {
        // Handle events
        quit = handle_events(spider, debug_effect);
        
        // Update time
        float current_time = gle::time();
        float elapsed_time = current_time - last_time;
        last_time = current_time;
        print_fps(elapsed_time);
        
        // Update the scene based on the time elapsed since last update
        spider.update(terrain, elapsed_time);
        Butterfly::spawn_and_destroy_and_update(scene, terrain);
        Firefly::spawn_and_destroy_and_update(scene);
        
        // Update the camera
        update_camera(camera, spider.get_position(terrain), spider.get_view_direction(terrain));
        directional_light->shadow_target = spider.get_position(terrain);
        
        // draw one frame
        camera.draw(scene);
        if(ssao_enabled)
            camera.apply_post_effect(ssao_effect);
        camera.apply_post_effect(fog_effect);
        camera.apply_post_effect(debug_effect);
        
        SDL_GL_SwapWindow(window);
    }
    
    // Delete context
    SDL_GL_DeleteContext(glcontext);
    
    // Destroy window
    SDL_DestroyWindow( window );
    window = NULL;
    
    // Quit SDL subsystems
    SDL_Quit();
    return 0;
}
