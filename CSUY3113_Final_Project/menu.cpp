
#include "menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 1
#define LEVEL_HEIGHT 1

constexpr char SPRITESHEET_FILEPATH[] = "assets/blue_car.png",
PLATFORM_FILEPATH[] = "assets/tileset.png",
ENEMY_FILEPATH[] = "assets/police_car.png";

unsigned int LEVEL0_DATA[] =
{
    1
};


Level_0::~Level_0()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeMusic(m_game_state.bgm);
}

void Level_0::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/chaseforpace_menu.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL0_DATA, map_texture_id, 7.6f, 1, 1);

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

    m_game_state.player->set_position(glm::vec3(6.0f, -5.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(0.0f);

     /*
      BGM and SFX
      */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/menu_music.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
}

void Level_0::update(float delta_time)
{
    /*m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }*/
}


void Level_0::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);

}