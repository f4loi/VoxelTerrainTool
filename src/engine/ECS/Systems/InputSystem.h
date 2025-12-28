/**
 * @file InputSystem.h
 * @brief Sistema de gestión de entrada (Input).
 * @date 2025-12-22
 */

#pragma once
#include <raylib.h>
#include "engine//ECS/Managers/EntityManager.h"

/**
 * @class InputSystem
 * @brief Sistema encargado de actualizar los componentes de input.
 */
class InputSystem
{
private:
public:
    /**
     * @brief Lee la entrada y actualiza las entidades.
     *
     * 1. Detecta si la tecla específica ('L') está presionada usando IsKeyDown.
     * 2. Itera sobre todas las entidades del EntityManager.
     * 3. Si una entidad tiene InputCMP, actualiza su estado interno isKeyLPressed.
     *
     * @param em Referencia al gestor de entidades para poder iterar y acceder a los componentes.
     */
    void Update(EntityManager &em)
    {

        bool isLPressed = IsKeyDown(KEY_L);

        em.forAll([&em, isLPressed](EntityManager::EntityID id)
                  {
            
            InputCMP* input = em.GetInput(id);
            
            if (input != nullptr)
            {
                
                input->isKeyLPressed = isLPressed;
            } });
    }
};