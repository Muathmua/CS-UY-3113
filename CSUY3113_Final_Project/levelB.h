#include "Scene.h"

class LevelB : public Scene {
public:
    // ����� STATIC ATTRIBUTES ����� //
    int ENEMY_COUNT = 10;
    bool time_over = false;
    // ����� DESTRUCTOR ����� //
    ~LevelB();

    // ����� METHODS ����� //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};