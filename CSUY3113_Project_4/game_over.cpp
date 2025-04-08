#include "game_over.h"
#include "Utility.h"

#define LEVEL_WIDTH 1
#define LEVEL_HEIGHT 1

constexpr char SPRITESHEET_FILEPATH[] = "assets/sonic_spritesheet.png",
PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png",
ENEMY_FILEPATH[] = "assets/crabmeat_sprite.png",
FONTSHEET_FILEPATH[] = "assets/font1.png";

//GLuint font_texture_id;

unsigned int LEVEL_LOSE_DATA[] =
{
    1
};


Level_Lose::~Level_Lose()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Level_Lose::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/game_over.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_LOSE_DATA, map_texture_id, 10.0f, 1, 1);

    //GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    //int player_animations[5][2] =
    //{
    //    { 0, 0 }, // for Sonic to face left
    //    { 1, 1 }, // for Sonic to face right
    //    { 5, 6 },  // for Sonic to move to the left,
    //    { 2, 3 }, // for Sonic to move to the right,
    //    { 4, 4 } // for Sonic to jump
    //};

    //glm::vec3 acceleration = glm::vec3(0.0f, -7.81f, 0.0f);

    //m_game_state.player = new Entity(
    //    player_texture_id,         // texture id
    //    5.0f,                      // speed
    //    acceleration,              // acceleration
    //    5.0f,                      // jumping power
    //    player_animations,  // animation index sets
    //    0.0f,                      // animation time
    //    2,                         // animation frame amount
    //    0,                         // current animation index
    //    5,                         // animation column amount
    //    2,                         // animation row amount
    //    1.0f,                      // width
    //    1.0f,                       // height
    //    PLAYER
    //);

    //m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));

    // Jumping
    //m_game_state.player->set_jumping_power(3.0f);

    /**
     Enemies' stuff */
     //GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

     //m_game_state.enemies = new Entity[ENEMY_COUNT];

     //for (int i = 0; i < ENEMY_COUNT; i++)
     //{
     //    m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
     //}


     //m_game_state.enemies[0].set_position(glm::vec3(10.0f, 0.0f, 0.0f));
     ////m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
     //m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

     /**
      BGM and SFX
      */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/menu_music.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
    //font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    //m_game_state.jump_sfx = Mix_LoadWAV("assets/sonic_jump.wav");
}

void Level_Lose::update(float delta_time)
{
    /*m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }*/
}


void Level_Lose::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    //Utility::draw_text(g_shader_program, font_texture_id, "Press Enter to begin!", 0.5f, 0.0f, glm::vec3(-3.7f, -3.2f, 0.0f));

    /* m_game_state.player->render(g_shader_program);
     for (int i = 0; i < m_number_of_enemies; i++)
         m_game_state.enemies[i].render(g_shader_program);*/
}
