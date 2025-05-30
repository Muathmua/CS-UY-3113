/**
* Author: Muath Alghamdi
* Assignment: Chase for Pace
* Date due: April 25, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 45
#define LEVEL_HEIGHT 13

constexpr char SPRITESHEET_FILEPATH[] = "assets/blue_car.png",
PLATFORM_FILEPATH[] = "assets/tileset.png",
ENEMY_FILEPATH[] = "assets/red_car.png";
float TIME_LIMIT = 49.0f;

float time_counter = TIME_LIMIT;
std::string time_text;

//tile values
// 
// Road block: 0
// Pavement:
//      Top: 7
//      Top left: 8
//      Top right: 12
// 
//      Left: 1
//      Right: 5
// 
//      Bottom: 35
//      Bottom left: 36
//      Bottom right: 40
//


unsigned int LEVEL_B_DATA[] =
{
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 0, 15, 9, 15, 9, 15,
    7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 0, 9, 15, 9, 15, 9,
    7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 7, 0, 7, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 9, 15, 9, 15,
    7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 15, 9, 15, 9,
    7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 9, 15, 9, 15,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
//  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 32 34 35 36 37 38 39 40 41 42 43
};





LevelB::~LevelB()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.countdown_sfx);
    Mix_FreeChunk(m_game_state.collide_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.tire_sfx);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_B_DATA, map_texture_id, 1.0f, 7, 7);

    time_counter = TIME_LIMIT;

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[8][1] =
    {
        { 6}, // left,
        { 2}, // right,
        { 0}, // upwards,
        { 4}, // downwards
        { 7}, // top left
        { 1}, // top right
        { 5}, // bottom left
        { 3}  // bottom right
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        1,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(7.0f, -6.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(0.0f);

    /**
     Enemies' stuff */
    set_number_of_enemies(10);
    int enemy_animation[8][1] =
    {
        { 6}, // left,
        { 2}, // right,
        { 0}, // upwards,
        { 4}, // downwards
        { 7}, // top left
        { 1}, // top right
        { 5}, // bottom left
        { 3}  // bottom right
    };
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    m_game_state.enemies->set_position(glm::vec3(6.0f, 0.0f, 0.0f));
   
    m_game_state.enemies->set_ai_type(GUARD);
    m_game_state.enemies->activate();

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
       m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, enemy_animation, 1.0f, 1.0f, ENEMY, BLOCKER, IDLE);
       m_game_state.enemies[i].set_animation_cols(8);
       m_game_state.enemies[i].set_animation_rows(1);
       m_game_state.enemies[i].set_animation_index(0);
       m_game_state.enemies[i].set_animation_time(0.0f);
       m_game_state.enemies[i].set_animation_frames(4);
       m_game_state.enemies[i].set_ai_type(BLOCKER);
       m_game_state.enemies[i].activate();
    }

    m_game_state.enemies[0].set_position(glm::vec3(10.0f, 0.0f, 0.0f));
    m_game_state.enemies[1].set_position(glm::vec3(12.0f, -1.0f, 0.0f));
    m_game_state.enemies[2].set_position(glm::vec3(14.0f, -5.0f, 0.0f));
    m_game_state.enemies[3].set_position(glm::vec3(16.0f, -10.0f, 0.0f));
    m_game_state.enemies[4].set_position(glm::vec3(20.0f, 0.0f, 0.0f));
    m_game_state.enemies[5].set_position(glm::vec3(20.0f, -13.0f, 0.0f));
    m_game_state.enemies[6].set_position(glm::vec3(29.0f, 0.0f, 0.0f));
    //m_game_state.enemies[7].set_position(glm::vec3(29.0f, 2.0f, 0.0f));
    //m_game_state.enemies[8].set_position(glm::vec3(29.0f, 4.0f, 0.0f));
    //m_game_state.enemies[9].set_position(glm::vec3(40.0f, 4.0f, 0.0f));


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/levelB_music.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
    m_game_state.countdown_sfx = Mix_LoadWAV("assets/countdown_sfx.wav");
    Mix_PlayChannel(-1, m_game_state.countdown_sfx, 0);
    Mix_VolumeChunk(m_game_state.countdown_sfx, MIX_MAX_VOLUME / 3);

    m_game_state.collide_sfx = Mix_LoadWAV("assets/collide_sfx.wav");
    m_game_state.engine_sfx = Mix_LoadWAV("assets/engine_sfx.wav");
    m_game_state.tire_sfx = Mix_LoadWAV("assets/tire_screech.wav");

}

void LevelB::update(float delta_time)
{
    time_counter -= 1 * delta_time;
    if (time_counter < 46) {
        if (time_counter < 0) { time_over = true; time_counter = TIME_LIMIT; }

        m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, LEVEL_HEIGHT);
        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, LEVEL_HEIGHT);
        }

    }
}


void LevelB::render(ShaderProgram* g_shader_program)
{
    time_text = std::to_string(int(time_counter));

    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);

    for (int i = 0; i < m_number_of_enemies; i++)
        m_game_state.enemies[i].render(g_shader_program);

    Utility::draw_text(g_shader_program, time_text, 0.5f, 0.0f,
        glm::vec3(m_game_state.player->get_position().x + 3.0f, m_game_state.player->get_position().y + 3.0f, 0.0f));
}