#pragma once

#include "../../engine/ECS/Managers/EntityManager.h"
#include "../../engine/ECS/Systems/RenderSystem.h"
#include "../../engine/ECS/Systems/PhysicSystem.h"
#include "../../engine/ECS/Systems/InputSystem.h"

class Game
{
private:
    EntityManager em;
    InputSystem inputSystem;
    RenderSystem renderSystem;
    PhysicSystem physicSystem;
    bool running = true;
public:
    Game() = default;
    void Update();
    void Render();
    void Init();
    void run();

    void createStarfield();
};