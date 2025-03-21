/**
* Author: Muath Alghamdi
* Assignment: Pong Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

// --- NAMING --- 
// This assignment has an UNO theme
// Blue refers to the blue uno card or right side player
// Red refers to the red uno card or left side player
// Draw, draw2, draw3 refer to each draw 4 card (or ball)

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <random>
#include <ctime>
#include <cstdlib>

enum AppStatus { RUNNING, TERMINATED };
constexpr float WINDOW_SIZE_MULT = 2.0f;
constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;
constexpr float BG_RED = 0.9765625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
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
constexpr char BLUE_SPRITE_FILEPATH[] = "uno_blue.jpg",
RED_SPRITE_FILEPATH[] = "uno_red.jpg",
DRAW_SPRITE_FILEPATH[] = "uno_draw.jpg",
DRAW2_SPRITE_FILEPATH[] = "draw2_yellow.png",
DRAW3_SPRITE_FILEPATH[] = "draw2_green.png",
BG_SPRITE_FILEPATH[] = "uno_background.jpg",
BLUE_WIN_SPRITE_FILEPATH[] = "uno_blue_win.png",
RED_WIN_SPRITE_FILEPATH[] = "uno_red_win.png";

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;

constexpr glm::vec3 INIT_SCALE_RED = glm::vec3(1.0f, 1.5f, 0.0f),
INIT_POS_RED = glm::vec3(-3.0f, 0.0f, 0.0f),
INIT_SCALE_BLUE = glm::vec3(1.0f, 1.5f, 0.0f),
INIT_POS_BLUE = glm::vec3(3.0f, 0.0f, 0.0f),
INIT_SCALE_DRAW = glm::vec3(0.5f, 0.75f, 0.0f),
INIT_POS_DRAW = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_POS_SCREEN = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_SCALE_SCREEN = glm::vec3(10.0f, 7.0f, 0.0f);

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_blue_matrix, g_projection_matrix, g_red_matrix, g_draw_matrix, g_draw_matrix2, g_draw_matrix3, g_screen_matrix;
float g_previous_ticks = 0.0f;
GLuint g_blue_texture_id;
GLuint g_red_texture_id;
GLuint g_draw_texture_id;
GLuint g_draw_texture_id2;
GLuint g_draw_texture_id3;
GLuint g_blue_win_texture_id;
GLuint g_red_win_texture_id;
GLuint g_bg_texture_id;

constexpr float SPEED = 3.0f;

float blue_y = 0.0f;
glm::vec3 g_blue_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_blue_movement = glm::vec3(0.0f, blue_y, 0.0f);
glm::vec3 g_red_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_red_movement = glm::vec3(0.0f, 0.0f, 0.0f);

float draw_x = 0.0;
float draw_y = 0.0;
glm::vec3 g_draw_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_draw_movement = glm::vec3(draw_x, draw_y, 0.0f);

float draw_x2 = 0.0;
float draw_y2 = 0.0;
glm::vec3 g_draw_position2 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_draw_movement2 = glm::vec3(draw_x2, draw_y2, 0.0f);

float draw_x3 = 0.0;
float draw_y3 = 0.0;
glm::vec3 g_draw_position3 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_draw_movement3 = glm::vec3(draw_x3, draw_y3, 0.0f);

float upper_wall = 3.0f;
float lower_wall = -3.0f;

float num_draws = 1.0;

float blue_score = 0;
float red_score = 0;
bool blue_win = false;
bool red_win = false;
bool single_player = false;

void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    return textureID;
}
void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Pong!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (g_display_window == nullptr) shutdown();
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_blue_matrix = glm::mat4(1.0f);
    g_red_matrix = glm::mat4(1.0f);
    g_draw_matrix = glm::mat4(1.0f);
    g_red_matrix = glm::translate(g_red_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    g_red_position += g_red_movement;
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    g_blue_texture_id = load_texture(BLUE_SPRITE_FILEPATH);
    g_red_texture_id = load_texture(RED_SPRITE_FILEPATH);
    g_draw_texture_id = load_texture(DRAW_SPRITE_FILEPATH);
    g_draw_texture_id2 = load_texture(DRAW2_SPRITE_FILEPATH);
    g_draw_texture_id3 = load_texture(DRAW3_SPRITE_FILEPATH);
    g_blue_win_texture_id = load_texture(BLUE_WIN_SPRITE_FILEPATH);
    g_red_win_texture_id = load_texture(RED_WIN_SPRITE_FILEPATH);
    g_bg_texture_id = load_texture(BG_SPRITE_FILEPATH);


    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_q: g_app_status = TERMINATED; break;

                // t toggles the single_player boolean, editing the y value of the blue card
            case SDLK_t: single_player = !single_player; blue_y = (single_player) ? 1.0f : 0.0f;

                // --- EXTRA CREDIT --- increasing number of draw cards (balls)
            case SDLK_1: num_draws = 1; break;
            case SDLK_2: num_draws = 2; break;
            case SDLK_3: num_draws = 3; break;
            default: break;
            }
        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    // Input processing for red and blue card movement
    if (key_state[SDL_SCANCODE_S]) {
        g_red_movement.y = -1.0;
    }
    else if (key_state[SDL_SCANCODE_W]) {
        g_red_movement.y = 1.0;
    }
    else {
        g_red_movement.y = 0.0;
    }

    if (!single_player) {
        if (key_state[SDL_SCANCODE_DOWN]) {
            blue_y = -1.0;
        }
        else if (key_state[SDL_SCANCODE_UP]) {
            blue_y = 1.0;
        }
        else {
            blue_y = 0;
        }
    }

}


void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    // Same translation and scaling for the background and win screens, so they all use one matrix
    g_screen_matrix = glm::mat4(1.0f);
    g_screen_matrix = glm::translate(g_screen_matrix, INIT_POS_SCREEN);
    g_screen_matrix = glm::scale(g_screen_matrix, INIT_SCALE_SCREEN);

    // If no one has won yet
    if (!red_win && !blue_win) {

        g_red_position += g_red_movement * SPEED * delta_time;
        g_blue_position += glm::vec3(0.0f, blue_y, 0.0f) * SPEED * delta_time;
        g_draw_position += glm::vec3(draw_x, draw_y, 0.0f) * SPEED * delta_time;

        g_blue_matrix = glm::mat4(1.0f);
        g_blue_matrix = glm::translate(g_blue_matrix, INIT_POS_BLUE);
        g_blue_matrix = glm::translate(g_blue_matrix, g_blue_position);

        g_red_matrix = glm::mat4(1.0f);
        g_red_matrix = glm::translate(g_red_matrix, INIT_POS_RED);
        g_red_matrix = glm::translate(g_red_matrix, g_red_position);

        g_draw_matrix = glm::mat4(1.0f);
        g_draw_matrix = glm::translate(g_draw_matrix, INIT_POS_DRAW);
        g_draw_matrix = glm::translate(g_draw_matrix, g_draw_position);

        g_blue_matrix = glm::scale(g_blue_matrix, INIT_SCALE_BLUE);
        g_red_matrix = glm::scale(g_red_matrix, INIT_SCALE_RED);
        g_draw_matrix = glm::scale(g_draw_matrix, INIT_SCALE_DRAW);


        // --- COLLISION --- 

        // Up and down movement for single-player 
        if (single_player) {
            if (lower_wall > g_blue_position.y || upper_wall < g_blue_position.y) {
                blue_y *= -1;
            }
        }

        float red_x_distance = fabs(g_red_position.x + INIT_POS_RED.x - g_draw_position.x + INIT_POS_DRAW.x) - ((INIT_SCALE_RED.x + INIT_SCALE_DRAW.x) / 2.0f);
        float red_y_distance = fabs(g_red_position.y + INIT_POS_RED.y - g_draw_position.y + INIT_POS_DRAW.y) - ((INIT_SCALE_RED.y + INIT_SCALE_DRAW.y) / 2.0f);
        if (red_x_distance < 0 && red_y_distance < 0) {
            // Moves draw(ball) slightly away from the card to avoid clipping as mentioned in class
            g_draw_position.x = INIT_POS_RED.x + 1.01f;
            // Reverses direction when collided
            draw_x *= -1;
            // Adds some randomness to y speed for more fun gameplay
            draw_y *= (1 + ((rand() % 4) / 2));
        }

        float blue_x_distance = fabs(g_blue_position.x + INIT_POS_BLUE.x - g_draw_position.x + INIT_POS_DRAW.x) - ((INIT_SCALE_BLUE.x + INIT_SCALE_DRAW.x) / 2.0f);
        float blue_y_distance = fabs(g_blue_position.y + INIT_POS_BLUE.y - g_draw_position.y + INIT_POS_DRAW.y) - ((INIT_SCALE_BLUE.y + INIT_SCALE_DRAW.y) / 2.0f);
        if (blue_x_distance < 0 && blue_y_distance < 0) {
            // Moves draw(ball) slightly away from the card to avoid clipping as mentioned in class
            g_draw_position.x = INIT_POS_BLUE.x - 1.01f;
            // Reverses direction when collided
            draw_x *= -1;
            // Adds some randomness to y speed for more fun gameplay
            draw_y *= (1 + ((rand() % 4 + 1) / 2));
        }

        // Colliding with lower and upper edges of screen
        if (lower_wall > g_draw_position.y) {
            g_draw_position.y = -2.9f;
            draw_y *= -1.2f;
        }
        if (upper_wall < g_draw_position.y) {
            g_draw_position.y = 2.9f;
            draw_y *= -1.2f;
        }

        // Limits the speed of the draw(ball) to make sure it doesn't go too crazy
        if (draw_y > 0.7f) {
            if (draw_y >= 0) {
                draw_y = 0.2f;
            }
            else {
                draw_y = -0.2f;
            }
        }

        // Detects if draw(ball) scored for either team
        if (6.0f < g_draw_position.x) {
            red_score += 1;
        }
        if (-6.0f > g_draw_position.x) {
            blue_score += 1;
        }

        // Stops paddles from going outside screen
        if (g_blue_position.y > 3.0f) {
            g_blue_position.y = 3.0f;
        }
        if (g_blue_position.y < -3.0f) {
            g_blue_position.y = -3.0f;
        }
        if (g_red_position.y > 3.0f) {
            g_red_position.y = 3.0f;
        }
        if (g_red_position.y < -3.0f) {
            g_red_position.y = -3.0f;
        }

        //--- (EXTRA CREDIT) ADDITIONAL BALL CODE (same code above, just done for each ball)----
        if (num_draws >= 2) {
            //There has got to be a better way to do this, right?
            g_draw_position2 += glm::vec3(draw_x2, draw_y2, 0.0f) * SPEED * delta_time;
            g_draw_matrix2 = glm::mat4(1.0f);
            g_draw_matrix2 = glm::translate(g_draw_matrix2, INIT_POS_DRAW);
            g_draw_matrix2 = glm::translate(g_draw_matrix2, g_draw_position2);
            g_draw_matrix2 = glm::scale(g_draw_matrix2, INIT_SCALE_DRAW);

            float red_x_distance = fabs(g_red_position.x + INIT_POS_RED.x - g_draw_position2.x + INIT_POS_DRAW.x) - ((INIT_SCALE_RED.x + INIT_SCALE_DRAW.x) / 2.0f);
            float red_y_distance = fabs(g_red_position.y + INIT_POS_RED.y - g_draw_position2.y + INIT_POS_DRAW.y) - ((INIT_SCALE_RED.y + INIT_SCALE_DRAW.y) / 2.0f);
            if (red_x_distance < 0 && red_y_distance < 0) {
                g_draw_position2.x = INIT_POS_RED.x + 1.01f;
                draw_x2 *= -1;
                draw_y2 *= (1 + ((rand() % 4) / 2));

            }
            float blue_x_distance = fabs(g_blue_position.x + INIT_POS_BLUE.x - g_draw_position2.x + INIT_POS_DRAW.x) - ((INIT_SCALE_BLUE.x + INIT_SCALE_DRAW.x) / 2.0f);
            float blue_y_distance = fabs(g_blue_position.y + INIT_POS_BLUE.y - g_draw_position2.y + INIT_POS_DRAW.y) - ((INIT_SCALE_BLUE.y + INIT_SCALE_DRAW.y) / 2.0f);
            if (blue_x_distance < 0 && blue_y_distance < 0) {
                g_draw_position2.x = INIT_POS_BLUE.x - 1.01f;
                draw_x2 *= -1;
                draw_y2 *= (1 + ((rand() % 4 + 1) / 2));

            }
            if (lower_wall > g_draw_position2.y) {
                g_draw_position2.y = -2.9f;
                draw_y2 *= -1.2f;
            }
            if (upper_wall < g_draw_position2.y) {
                g_draw_position2.y = 2.9f;
                draw_y2 *= -1.2f;
            }
            if (draw_y2 > 0.7f) {
                if (draw_y2 >= 0) {
                    draw_y2 = 0.2f;
                }
                else {
                    draw_y2 = -0.2f;
                }
            }
            if (6.0f < g_draw_position2.x) {
                red_score += 1;
            }
            if (-6.0f > g_draw_position2.x) {
                blue_score += 1;
            }
        }
        if (num_draws == 3) {
            g_draw_position3 += glm::vec3(draw_x3, draw_y3, 0.0f) * SPEED * delta_time;
            g_draw_matrix3 = glm::mat4(1.0f);
            g_draw_matrix3 = glm::translate(g_draw_matrix3, INIT_POS_DRAW);
            g_draw_matrix3 = glm::translate(g_draw_matrix3, g_draw_position3);
            g_draw_matrix3 = glm::scale(g_draw_matrix3, INIT_SCALE_DRAW);

            float red_x_distance = fabs(g_red_position.x + INIT_POS_RED.x - g_draw_position3.x + INIT_POS_DRAW.x) - ((INIT_SCALE_RED.x + INIT_SCALE_DRAW.x) / 2.0f);
            float red_y_distance = fabs(g_red_position.y + INIT_POS_RED.y - g_draw_position3.y + INIT_POS_DRAW.y) - ((INIT_SCALE_RED.y + INIT_SCALE_DRAW.y) / 2.0f);
            if (red_x_distance < 0 && red_y_distance < 0) {
                g_draw_position3.x = INIT_POS_RED.x + 1.01f;
                draw_x3 *= -1;
                draw_y3 *= (1 + ((rand() % 3) / 2));
            }
            float blue_x_distance = fabs(g_blue_position.x + INIT_POS_BLUE.x - g_draw_position3.x + INIT_POS_DRAW.x) - ((INIT_SCALE_BLUE.x + INIT_SCALE_DRAW.x) / 2.0f);
            float blue_y_distance = fabs(g_blue_position.y + INIT_POS_BLUE.y - g_draw_position3.y + INIT_POS_DRAW.y) - ((INIT_SCALE_BLUE.y + INIT_SCALE_DRAW.y) / 2.0f);
            if (blue_x_distance < 0 && blue_y_distance < 0) {
                g_draw_position3.x = INIT_POS_BLUE.x - 1.01f;
                draw_x3 *= -1;
                draw_y3 *= (1 + ((rand() % 3 + 1) / 2));
            }
            if (lower_wall > g_draw_position3.y) {
                g_draw_position3.y = -2.9f;
                draw_y3 *= -1.2f;
            }
            if (upper_wall < g_draw_position3.y) {
                g_draw_position3.y = 2.9f;
                draw_y3 *= -1.2f;
            }
            if (draw_y3 > 0.7f) {
                if (draw_y3 >= 0) {
                    draw_y3 = 0.2f;
                }
                else {
                    draw_y3 = -0.2f;
                }
            }
            if (6.0f < g_draw_position3.x) {
                red_score += 1;
            }
            if (-6.0f > g_draw_position3.x) {
                blue_score += 1;
            }
        }
    }

    // Win check
    if (blue_score >= 1) {
        blue_win = true;

    }
    if (red_score >= 1) {
        red_win = true;
    }

}
void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}
void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    // Vertices
    float vertices[] = {
      -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, // triangle 1
      -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f  // triangle 2s
    };
    // Textures
    float texture_coordinates[] = {
      0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,   // triangle 1
      0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // triangle 2
    };
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    // Bind texture
    draw_object(g_screen_matrix, g_bg_texture_id);

    draw_object(g_red_matrix, g_red_texture_id);
    draw_object(g_blue_matrix, g_blue_texture_id);
    draw_object(g_draw_matrix, g_draw_texture_id);

    // Renders extra balls only when they are activated
    if (num_draws >= 2) {
        draw_object(g_draw_matrix2, g_draw_texture_id2);
    }
    if (num_draws == 3) {
        draw_object(g_draw_matrix3, g_draw_texture_id3);
    }

    // Renders win screen
    if (blue_win) {
        draw_object(g_screen_matrix, g_blue_win_texture_id);
    }
    if (red_win) {
        draw_object(g_screen_matrix, g_red_win_texture_id);
    }

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    SDL_GL_SwapWindow(g_display_window);
}
void shutdown() { SDL_Quit(); }
int main(int argc, char* argv[])
{

    //Using randomness to add variation to the beginning
    srand(static_cast<unsigned int>(time(0)));
    draw_x = (rand() % 2 == 0) ? 1.0 : -1.0;
    draw_y = (rand() % 2 == 0) ? 0.2 : -0.2;

    draw_x2 = (rand() % 2 == 0) ? 1.0 : -1.0;
    draw_y2 = (rand() % 2 == 0) ? 0.2 : -0.2;

    draw_x3 = (rand() % 2 == 0) ? 1.0 : -1.0;
    draw_y3 = (rand() % 2 == 0) ? 0.2 : -0.2;

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