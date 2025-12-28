#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <cstdint>

// -----------------------------------------------------------------------------
// FUNCION AUXILIAR (Lógica del Juego)
// -----------------------------------------------------------------------------
// Esta función sustituye a la lambda que tenías en Update().
// Debe ser una función libre (o estática) para poder pasarse a em.forAll.
void GameLogicOneEntity(Entity ent)
{
    EntityManager* em = ent.getParent();
    // Asumimos que getNextId devuelve el índice correcto (tu ID)
    int id = ent.getNextId(); 

    if (em != nullptr)
    {
        InputCMP* input = em->GetInput(id);
        RenderCMP* render = em->GetRender(id);

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
    // Inicializamos el sistema de render (abre la ventana, carga recursos...)
    renderSystem.Init();
    
    // Creamos las entidades iniciales
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
    srand(static_cast<unsigned int>(time(nullptr)));
    const uint16_t starCount = 70;

    for (uint16_t i = 0; i < starCount; i += 1)
    {
       
        Entity::EntityID starId = em.CreateEntity();

       
        PhysicCMP physic{};
        physic.x = static_cast<float>(rand() % 800); 
        physic.y = static_cast<float>(rand() % 600); 
        
       
      
        
        
        em.AddPhysic(starId, physic);

      
        RenderCMP render{};
        render.isRendered = true;
        em.AddRender(starId, render);

        InputCMP input{};
        input.isKeyLPressed = false;
        em.AddInput(starId, input);
    }
}