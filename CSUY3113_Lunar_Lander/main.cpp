/**
* Author: Muath Alghamdi
* Assignment: Lunar Lander
* Date due: 2025-3-20, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define PLATFORM_COUNT 15

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include <cstdlib>
#include "Entity.h"

enum AppStatus { RUNNING, TERMINATED };

struct GameState
{
    Entity* player;
    Entity* platforms;
    Entity* ui;
};

// ––––– CONSTANTS ––––– //
constexpr float WINDOW_SIZE_MULT = 2.0f;
constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;
constexpr float BG_RED = 0.0f,
BG_GREEN = 0.0f,
BG_BLUE = 0.0f,
BG_OPACITY = 1.0f;
constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;
constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;
constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char LUNAR_LANDER_SPRITE_FILEPATH[] = "assets/Lunar_Lander.png",
//LUNAR_LANDER_ON_SPRITE_FILEPATH[] = "assets/Lunar_Lander_On.png",
PLATFORM_FILEPATH[] = "assets/rock_tile_.png",
WIN_SPRITE_FILEPATH[] = "assets/win.png",
FAIL_SPRITE_FILEPATH[] = "assets/fail.png";
constexpr glm::vec3 INIT_POS_SCREEN = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_SCALE_SCREEN = glm::vec3(10.0f, 7.0f, 0.0f);


//constexpr glm::vec3 INIT_POS_SCREEN = glm::vec3(0.0f, 0.0f, 0.0f),
//INIT_SCALE_SCREEN = glm::vec3(10.0f, 7.0f, 0.0f);

AppStatus g_app_status = RUNNING;
SDL_Window* g_display_window;

ShaderProgram g_shader_program;

GameState g_state;

bool g_game_is_running = true;

bool mission_accomplished = false;
bool mission_fail = false;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix, g_screen_matrix;
GLuint g_win_texture_id;
GLuint g_fail_texture_id;
float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool out_of_fuel = false;

float fuel = 5000.0f;



const Uint8* key_state = SDL_GetKeyboardState(NULL);

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Physics (again)!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ––––– VIDEO ––––– //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ––––– PLATFORMS ––––– //
    GLuint platform_texture_id = load_texture(PLATFORM_FILEPATH);

    g_state.platforms = new Entity[PLATFORM_COUNT];



    // Making specific platforms the win platforms
    // Was initially going to make this using rand() for variation,
    // But it was too inconsistent

    int index_of_win_tile = 4;
    int index_of_win_tile2 = 9;

    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        g_state.platforms[i].set_texture_id(platform_texture_id);

        if (i != index_of_win_tile && i != index_of_win_tile2) {
            g_state.platforms[i].set_position(glm::vec3(i - PLATFORM_COUNT / 2.0f, -3.0f, 0.0f));
        }
        // If it is the win platform, set it as such, and change the position so it is visible
        else {
            g_state.platforms[i].set_win_tile();
            g_state.platforms[i].set_position(glm::vec3(i - PLATFORM_COUNT / 2.0f, -2.5f, 0.0f));
        }
        g_state.platforms[i].set_width(1.0f);
        g_state.platforms[i].set_height(1.0f);
        g_state.platforms[i].set_entity_type(PLATFORM);
        g_state.platforms[i].update(0.0f, NULL, NULL, 0);
    }

    g_win_texture_id = load_texture(WIN_SPRITE_FILEPATH);
    g_state.ui = new Entity[2];
    g_state.ui[0].set_texture_id(g_win_texture_id);

    g_fail_texture_id = load_texture(FAIL_SPRITE_FILEPATH);
    g_state.ui[1].set_texture_id(g_fail_texture_id);

    GLuint player_texture_id = load_texture(LUNAR_LANDER_SPRITE_FILEPATH);

    glm::vec3 acceleration = glm::vec3(0.0f, -2.0f, 0.0f);
    g_state.player = new Entity;
    g_state.player->set_position(glm::vec3(0.0f, 3.0f, 0.0f));
    g_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    g_state.player->set_texture_id(player_texture_id);
    g_state.player->set_speed(5.0f);
    g_state.player->set_width(1.0f);
    g_state.player->set_height(1.0f);

    
    

    // ––––– GENERAL ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            default:
;                break;
            }

        default:
            break;
        }
    }

    if (!out_of_fuel) {
        if (key_state[SDL_SCANCODE_LEFT])
        {
            g_state.player->set_acceleration_x(-50.0f);
        }

        if (key_state[SDL_SCANCODE_RIGHT])
        {
            g_state.player->set_acceleration_x(50.0f);
        }

        if (key_state[SDL_SCANCODE_UP])
        {
            g_state.player->set_acceleration_y(2.0f);
        }

        if (glm::length(g_state.player->get_movement()) > 1.0f)
        {
            g_state.player->normalise_movement();
        }
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    // If collided with the win platform, end the game

    if (g_state.player->get_collided_bottom()) {
        fuel = -1;

        if (g_state.player->get_collided_win()) {
            mission_accomplished = true;
        }
        else {
            mission_fail = true;
        }

    }

    if (key_state[SDL_SCANCODE_LEFT]) {
        fuel -= 1.0f;
    }
    if (key_state[SDL_SCANCODE_RIGHT]) {
        fuel -= 1.0f;
    }    
    if (key_state[SDL_SCANCODE_UP]) {
        fuel -= 1.0f;
    }

    if (fuel <= 0.0f) {
        out_of_fuel = true;
    }

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        g_state.platforms->update(FIXED_TIMESTEP, NULL, g_state.player, 1);
        g_state.player->update(FIXED_TIMESTEP, NULL, g_state.platforms, PLATFORM_COUNT);
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;
}


void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (mission_accomplished == true) {
        g_state.ui[0].render(&g_program);
    }
    if (mission_fail == true) {
        g_state.ui[1].render(&g_program);
    }

    g_state.player->render(&g_program);
    for (int i = 0; i < PLATFORM_COUNT; i++) g_state.platforms[i].render(&g_program);
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    //delete g_state.ui;
    delete[] g_state.platforms;
    delete g_state.player;
}

// ––––– GAME LOOP ––––– //
int main(int argc, char* argv[])
{

    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}