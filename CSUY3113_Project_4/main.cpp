/**
* Author: Muath Alghamdi
* Assignment: Rise of the AI
* Date due: 2025-04-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
* --------------------------------------------------------
* KNOWN WEIRD BEHAVIOR (BUGS?)
* --------------------------------------------------------
* 1. Sometimes enemies require multiple switches to a scene to spawn.
*    This was also present in code recieved from the lectures, so it's likely an issue on my PC.
* 2. If played on a laptop whose performance decreases when not plugged to a charger, sonic's acceleration
*    is bugged.
**/


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "game_over.h"
#include "game_win.h"
// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

constexpr float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
Level_0* g_menu;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC* g_level_c;
Level_Lose* g_level_lose;
Level_Win* g_level_win;
SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

int g_lives = 3;
float damage_timer = 0.0f;
bool controls_off = true;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Scenes!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    
    // ————— LEVEL A SETUP ————— //
    g_menu = new Level_0();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    g_level_win = new Level_Win();
    g_level_lose = new Level_Lose();

    switch_to_scene(g_menu);
   

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    if (g_current_scene != g_menu) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                    case SDLK_0:
                        switch_to_scene(g_menu);
                        break;
                    case SDLK_RETURN:
                        switch_to_scene(g_level_a);
                        break;
                    case SDLK_1:
                        switch_to_scene(g_level_a);
                        break;
                    case SDLK_2:
                        switch_to_scene(g_level_b);
                        break;
                    case SDLK_3:
                        switch_to_scene(g_level_c);
                        break;
                    case SDLK_SPACE:
                        // ————— JUMPING ————— //
                        if (!controls_off) {
                            if (g_current_scene->get_state().player->get_collided_bottom())
                            {
                                g_current_scene->get_state().player->jump();
                                Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                            }
                        }
                         break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    if (!controls_off) {
        if (key_state[SDL_SCANCODE_DOWN]) {
            g_current_scene->get_state().player->roll();
            Mix_PlayChannel(-1, g_current_scene->get_state().spin_dash_sfx, 0);
        }
        if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
        else if (key_state[SDL_SCANCODE_A]) {
            g_current_scene->get_state().player->spin_dash_left();
            Mix_PlayChannel(-1, g_current_scene->get_state().spin_dash_sfx, 0);
        }
        else if (key_state[SDL_SCANCODE_D]) {
            g_current_scene->get_state().player->spin_dash_right();
            Mix_PlayChannel(-1, g_current_scene->get_state().spin_dash_sfx, 0);
        }
        else if (key_state[SDL_SCANCODE_Y]) switch_to_scene(g_level_b);

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //

        if (g_current_scene == g_menu || g_current_scene == g_level_win || g_current_scene == g_level_lose) {
            controls_off = true;
        }
        else {
            controls_off = false;
        }

        g_current_scene->update(FIXED_TIMESTEP);

        if (!controls_off) {

            if (g_current_scene->get_state().player->get_position().x >= 84.0f) {
                if (g_current_scene == g_level_a) {
                    switch_to_scene(g_level_b);
                }

                else if (g_current_scene == g_level_b) {
                    switch_to_scene(g_level_c);
                }

                else if (g_current_scene == g_level_c) {
                    switch_to_scene(g_level_win);
                }
            }

            if (damage_timer <= 0.0f && g_current_scene->get_state().player->took_damage()) {
                g_lives -= 1;
                g_current_scene->get_state().player->set_position(glm::vec3(5.0f, -3.0f, 0.0f));
                g_current_scene->get_state().player->set_took_damage(false);
                damage_timer = 1.0f; // 1 second invincibility
            }

            if (damage_timer > 0.0f) {
                damage_timer -= delta_time; // delta_time is time since last frame
            }

            if (g_lives <= 0) {
                switch_to_scene(g_level_lose);
                g_lives = 3;
            }
        }

        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    if (!controls_off) {
        if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
    }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{    
    SDL_Quit();
    
    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
