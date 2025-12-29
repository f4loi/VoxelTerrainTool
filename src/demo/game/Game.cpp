#include "Game.h"
#include "utils/random/helpersRandom.h"

void GameLogicOneEntity(Entity ent)
{
    EntityManager *em = ent.getParent();
    int id = ent.getNextId();

    if (em != nullptr)
    {
        InputCMP *input = em->GetInput(id);
        RenderCMP *render = em->GetRender(id);

        // Si la entidad tiene ambos componentes, aplicamos la lógica
        if (input != nullptr && render != nullptr)
        {
            // Lógica: Si se pulsa la tecla (detectada por InputSystem),
            // ocultamos el renderizado.
            render->isRendered = !input->isKeyLPressed;
        }
    }
}

// -----------------------------------------------------------------------------
// IMPLEMENTACIÓN DE LA CLASE GAME
// -----------------------------------------------------------------------------

void Game::Init()
{
    RandomHelpers::Random::Init();
    renderSystem.Init();

    createStarfield();
}

void Game::Update()
{
    // 1. INPUT: Detecta teclas y actualiza InputCMP
    inputSystem.Update(em);

    // 2. LÓGICA DE JUEGO:
    // Usamos la función auxiliar en lugar de la lambda.
    em.forAll(&GameLogicOneEntity);

    // 3. FÍSICAS: Mueve las entidades según su velocidad
    physicSystem.Update(em);
}

void Game::Render()
{
    // Dibuja todo lo que tenga RenderCMP activo
    renderSystem.Update(em);
}

void Game::run()
{
    Init();

    // Bucle principal del juego
    // (Asegúrate de tener un mecanismo para poner 'running' a false,
    //  por ejemplo detectando si se cierra la ventana en InputSystem o aquí)
    while (running)
    {
        Update();
        Render();
    }
}

void Game::createStarfield()
{

    const uint16_t starCount = 70;

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