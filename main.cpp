/**
* Author: Muath Alghamdi
* Assignment: Simple 2D Scene
* Date due: 2025-02-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
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

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 1.0f;
constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED = 1.0f,
                BG_GREEN = 1.0f,
                BG_BLUE = 1.0f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;


SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();


//larry is the black cat
constexpr char LARRY_SPRITE_FILEPATH[] = "evil_larry.jpg",
               CAT_SPRITE_FILEPATH[] = "cat.jpg";

//definitions for transformations
float g_larry_size = 1.0f;
float g_larry_y = 0.0f;

float g_cat_size = 1.0f;
float g_cat_x = -2.0f;
float g_cat_y = 0.0f;

//counter used with delta time to make a movement pattern
float counter = 0;

glm::mat4 g_view_matrix, g_larry_matrix, g_projection_matrix, g_cat_matrix;
glm::vec3 g_larry_scale;
glm::vec3 g_cat_scale;

float g_previous_ticks = 0.0f;

GLuint g_larry_texture_id;
GLuint g_cat_texture_id;

glm::vec3 g_larry_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_cat_rotation = glm::vec3(0.0f, 0.0f, 0.0f);

void initialise();
void process_input();
void update();
void render();
void shutdown();

constexpr GLint NUMBER_OF_TEXTURES = 1;  // to be generated, that is
constexpr GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
constexpr GLint TEXTURE_BORDER = 0;  // this value MUST be zero

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        std::cout << ("Cannot load image!");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("'larry' and 'cat' don't look like real words anymore.",
                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT,
                       SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_view_matrix = glm::mat4(1.0f);

    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    g_larry_texture_id = load_texture(LARRY_SPRITE_FILEPATH);
    g_cat_texture_id = load_texture(CAT_SPRITE_FILEPATH);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//Allows for quitting by closing the window
void process_input()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {

        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;
        }
    }
}

void update()
{

    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; 
    float delta_time = ticks - g_previous_ticks; 
    g_previous_ticks = ticks;
    counter += 1 * delta_time;

    //Creates a loop for translation and scaling pattern
    if (counter <= 1) {
        g_larry_size += 0.01f;
        g_larry_y -= 2.0f * delta_time;
        g_cat_x += 1.0f * delta_time;
        g_cat_y += 1.0f * delta_time;

    }
    if (counter >= 1) {
        g_larry_size -= 0.01f;
        g_larry_y += 2.0f * delta_time;
        g_cat_x -= 1.0f * delta_time;
        g_cat_y -= 1.0f * delta_time;
    }
    if (counter >= 2) {
        counter = 0;
    }

    g_larry_matrix = glm::mat4(1.0f);
    g_cat_matrix = glm::mat4(1.0f);

    //Translating -- Larry moves relative to cat
    g_larry_matrix = glm::translate(g_cat_matrix, glm::vec3(g_cat_x + 3.0f, g_larry_y, 0.0f));
    g_cat_matrix = glm::translate(g_cat_matrix, glm::vec3(g_cat_x, g_cat_y, 0.0f));

    //Scaling -- EXTRA CREDIT
    g_larry_scale = glm::vec3(g_larry_size, g_larry_size, 1.0f);
    g_larry_matrix = glm::scale(g_larry_matrix, g_larry_scale);

    //Rotating
    g_larry_rotation.y += 1.0f * delta_time * -1;
    g_larry_matrix = glm::rotate(g_larry_matrix, g_larry_rotation.y, glm::vec3(1.0f, 1.0f, 1.0f));
        
    //EXTRA CREDIT
    g_cat_rotation.y += 1.0f * delta_time * -1;
    g_cat_matrix = glm::rotate(g_cat_matrix, g_cat_rotation.y, glm::vec3(1.0f, 0.0f, 0.0f));

}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); 
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f 
    };

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    //Binding the textures
    draw_object(g_larry_matrix, g_larry_texture_id);
    draw_object(g_cat_matrix, g_cat_texture_id);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


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
