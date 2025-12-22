/**
 * @file Game.h
 * @brief Clase para probar el game engine creado
 * @date 22-12-2025
 */

#pragma once

#include "../../engine/ECS/Managers/EntityManager.h"
#include "../../engine/ECS/Systems/RenderSystem.h"
#include "../../engine/ECS/Systems/PhysicSystem.h"
#include "../../engine/ECS/Systems/InputSystem.h"

/**
 * @class Game
 * @brief Clase principal que encapsula el bucle del juego (Game Loop).
 *
 * Esta clase actúa como el mundo. Es responsable de:
 * 1. Instanciar el EntityManager.
 * 2. Instanciar y coordinar los Sistemas (Input, Render, Physics).
 * 3. Ejecutar el bucle principal: Init -> (Update -> Render) -> End.
 */
class Game
{
private:
    /**
     * @brief Manager de entidades.
     * Contiene todos los componentes y los IDs de las entidades del juego.
     */
    EntityManager em;

    InputSystem inputSystem;   ///< Sistema encargado de leer teclado/ratón.
    RenderSystem renderSystem; ///< Sistema encargado de dibujar en pantalla.
    PhysicSystem physicSystem; ///< Sistema encargado del movimiento.

    /**
     * @brief bool del bucle principal.
     * Mientras sea true, el juego seguirá ejecutando Update() y Render().
     */
    bool running = true;

public:
    /**
     * @brief Constructor por defecto.
     */
    Game() = default;

    /**
     * @brief Actualiza la lógica del juego .
     *
     * Llama a los sistemas que modifican el estado del juego:
     * - InputSystem::Update: Procesa entradas.
     * - Lógica de juego.
     * - PhysicSystem::Update: Mueve las entidades.
     */
    void Update();

    /**
     * @brief Dibuja la escena actual.
     *
     * Llama a RenderSystem::Update para visualizar las entidades
     * que tengan componente de renderizado.
     */
    void Render();

    /**
     * @brief Inicializa los recursos necesarios antes del bucle.
     *
     * Configura la ventana gráfica y crea las entidades iniciales.
     */
    void Init();

    /**
     * @brief Ejecuta el ciclo de vida completo del juego.
     *
     * 1. Llama a Init().
     * 2. Entra en un bucle while(running) llamando a Update() y Render().
     */
    void run();

    /**
     * @brief Método auxiliar para generar contenido de prueba.
     *
     * Crea un conjunto de entidades con componentes de Física, Render e Input
     * distribuidas aleatoriamente por la pantalla.
     * @note Utilizado internamente durante Init().
     */
    void createStarfield();
};