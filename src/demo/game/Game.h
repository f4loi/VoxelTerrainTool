#pragma once

#include "../../engine/ECS/Managers/EntityManager.h"
#include "../../engine/ECS/Systems/RenderSystem.h"

class Game
{
private:
    EntityManager em;
    RenderSystem renderSystem;
    bool running = true;
public:
    Game() = default;
    void Update();
    void Render();
    void Init();
    void run();

    void createStarfield();
};