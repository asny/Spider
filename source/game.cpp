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

bool ssao_enabled = false;
float zoom = 0.;
float target = 0.;

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
    if(abs(target - zoom) > 0.001)
    {
        zoom += sign(target - zoom) * 0.01;
    }
    vec3 third_person_view = normalize(vec3(spider_view_direction.x, -0.75, spider_view_direction.z));
    vec3 pos, dir;
    if(zoom < 1.f)
    {
        float f = 1.f - (1.f - zoom) * (1.f - zoom);
        pos = (spider_position - 0.5f * spider_view_direction + vec3(0.,0.4,0.)) * (1.f-zoom) +
        (spider_position - 5.f * third_person_view) * zoom;
        dir = normalize(spider_view_direction * (1.f-f) + third_person_view * f);
    }
    else {
        
        pos = (spider_position - 20.f * third_person_view) * (zoom - 1.f) + (spider_position - 5.f * third_person_view) * (2.f - zoom);
        dir = third_person_view;
        
    }
    
    camera.set_view(pos, dir);
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
        if( e.type == SDL_MOUSEWHEEL)
        {
            zoom += e.wheel.y / 100.f;
            zoom = clamp(zoom, 0.f, 2.f);
            target = zoom;
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
                target = 0.f;
                break;
            case SDLK_2:
                target = 1.f;
                break;
            case SDLK_3:
                target = 2.f;
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
    auto fog_effect = GLFogEffect();
    fog_effect.density = 0.08;
    fog_effect.no_fog_height = 6.;
    fog_effect.animation = 0.05;
    auto debug_effect = GLDebugEffect();
    
    // Create scene
    auto scene = GLScene();
    
    // Create objects
    auto environment = Environment(&scene);
    auto spider = Spider(&scene, glm::vec3(0., 0., -5.), glm::vec3(0., 0., 1.));
    
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
        spider.update(environment, elapsed_time);
        Butterfly::spawn_and_destroy_and_update(scene, environment, spider);
        Firefly::spawn_and_destroy_and_update(scene);
        
        // Update the camera
        update_camera(camera, spider.get_position(environment), spider.get_view_direction(environment));
        directional_light->shadow_target = spider.get_position(environment);
        
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
