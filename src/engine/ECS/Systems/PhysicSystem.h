/**
 * @file PhysicSystem.h
 * @brief Sistema físicas.
 * @date 22-12-2025
 */

#pragma once

#include "../../ECS/Managers/EntityManager.h"

/**
 * @class PhysicSystem
 * @brief Sistema encargado de actualizar la posición y física de las entidades.
 *
 * Este sistema itera sobre todas las entidades que poseen un `PhysicCMP`.
 * Su funcion principal es:
 * 1. Aplicar la lógica de movimiento (actualizar posición basada en velocidad).
 * 2. Mantener a las entidades dentro de los límites del mundo.
 */
class PhysicSystem
{
private:

public:
    /**
     * @brief Ejecuta la fisica de un frame.
     *
     * Recorre el EntityManager y modifica los componentes PhysicCMP de las entidades activas.
     * Actualiza x.
     *
     * @param em Referencia al manager de entidades. Se pasa por referencia no constante
     * porque este sistema necesita modificar los datos de los componentes.
     */
    void Update(EntityManager &em);
};