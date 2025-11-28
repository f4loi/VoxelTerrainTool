#include "Game.h"
#include <cstdlib>
#include <ctime>

void Game::Init()
{
    renderSystem.Init();
    createStarfield();
}

void Game::Update()
{
    physicSystem.Update(em);
}

void Game::Render()
{
    renderSystem.Update(em);
}

void Game::run()
{
    Init();
    while (running)
    {
        Update();
        Render();
    }
}

void Game::createStarfield()
{
    srand(time(nullptr));
    const int starCount = 70;
    for (int i = 0; i < starCount; ++i)
    {
        auto& star = em.CreateEntity();
        star.GetPhysicCMP().x = static_cast<float>(rand() % 800);
        star.GetPhysicCMP().y = static_cast<float>(rand() % 600);

    }
}