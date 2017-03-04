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

#define SDL_MAIN_HANDLED
#include "SDL.h"

using namespace std;
using namespace glm;
using namespace gle;
using namespace mesh;
using namespace std::chrono;

enum VIEW_TYPE { FIRST_PERSON, THIRD_PERSON, BIRD, WORM };

VIEW_TYPE view_type = FIRST_PERSON;

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

bool handle_events(Spider& spider)
{
    SDL_Event e;
    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
        {
            return true;
        }
        if( e.type == SDL_KEYDOWN )
        {
            switch( e.key.keysym.sym )
            {
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
                case SDLK_SPACE:
                    spider.jump();
                break;
                default:
                break;
            }
            
        }
        
        switch( e.key.keysym.sym )
        {
            case SDLK_UP:
            spider.move_foward(e.type == SDL_KEYDOWN);
            break;
            
            case SDLK_DOWN:
            spider.move_backward(e.type == SDL_KEYDOWN);
            break;
            
            case SDLK_LEFT:
            spider.rotate_left(e.type == SDL_KEYDOWN);
            break;
            
            case SDLK_RIGHT:
            spider.rotate_right(e.type == SDL_KEYDOWN);
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
    auto fog_effect = std::make_shared<GLFogEffect>(make_shared<GLTexture2D>("resources/water_noise.jpg"));
    camera.add_post_effect(fog_effect);
    
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
    
    bool quit = false;
    float last_time = gle::time();
    
    //While application is running
    while( !quit )
    {
        // Handle events
        quit = handle_events(spider);
        
        // Update time
        float current_time = gle::time();
        float elapsed_time = current_time - last_time;
        last_time = current_time;
        print_fps(elapsed_time);
        
        // Update the scene based on the time elapsed since last update
        spider.update(elapsed_time);
        Butterfly::spawn_and_destroy_and_update(scene);
        terrain.update(spider.get_position());
        
        // Update the camera
        update_camera(camera, spider.get_position(), spider.get_view_direction());
        
        // draw one frame
        camera.draw(scene);
        
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
