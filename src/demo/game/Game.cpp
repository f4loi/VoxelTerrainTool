#include "Game.h"

void Game::Init()
{
    renderSystem.Init();
    em.CreateEntity();
}

void Game::Update()
{
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