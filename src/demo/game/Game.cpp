#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <cstdint>

void Game::Init()
{
    renderSystem.Init();
    createStarfield();
}

void Game::Update()
{

    inputSystem.Update(em);

    em.forAll([&](EntityManager::EntityID id)
              {
        InputCMP* input = em.GetInput(id);
        RenderCMP* render = em.GetRender(id);

        if (input != nullptr && render != nullptr)
        {
           
            render->isRendered = !input->isKeyLPressed;
        } });

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
    //pasar a helpers
    srand(time(nullptr));
    const uint16_t starCount = 70;

    for (uint16_t i{}; i < starCount; i +=1)
    {
        auto starId = em.CreateEntity();

        PhysicCMP physic{};
        physic.x = static_cast<float>(rand() % 800);
        physic.y = static_cast<float>(rand() % 600);
        em.AddPhysic(starId, physic);

        RenderCMP render{};
        render.isRendered = true;
        em.AddRender(starId, render);

        InputCMP input{};
        em.AddInput(starId, input);
    }
}