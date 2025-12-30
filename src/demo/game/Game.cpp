#include "Game.h"
#include "utils/random/helpersRandom.h"

void Game::Init()
{
    RandomHelpers::Random::Init();
    renderSystem.Init();

    createStarfield();
}

void Game::LogicaOneEntity(Entity ent)
{
    EntityManager *em = ent.getParent();
    uint16_t id = ent.getNextId();

    if (em != nullptr)
    {
        InputCMP *input = em->GetInput(id);
        RenderCMP *render = em->GetRender(id);

        if (input != nullptr && render != nullptr)
        {

            render->isRendered = !input->isKeyLPressed;
        }
    }
}

void Game::Update()
{
    inputSystem.Update(em);
    em.forAll(&LogicaOneEntity);
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

    const uint16_t starCount = 95;

    for (uint16_t i = 0; i < starCount; i += 1)
    {
        Entity::EntityID starId = em.CreateEntity();

        PhysicCMP physic{};

        physic.x = RandomHelpers::Random::Float(0.0f, 800.0f);
        physic.y = RandomHelpers::Random::Float(0.0f, 600.0f);

        em.AddPhysic(starId, physic);

        RenderCMP render{};
        render.isRendered = true;
        em.AddRender(starId, render);

        InputCMP input{};
        input.isKeyLPressed = false;
        em.AddInput(starId, input);
    }
}