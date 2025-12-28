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
    void Update(EntityManager &em);

    static void UpdateOneEntity(Entity ent);
};