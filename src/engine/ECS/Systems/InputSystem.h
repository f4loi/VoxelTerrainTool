#pragma once
#include <raylib.h>
#include "../../ECS/Managers/EntityManager.h"

class InputSystem
{
private:

public:
    void Update(EntityManager& em)
    {
        if (IsKeyDown(KEY_L))
        {
            em.forAll([](Entity& entity)
            {
                entity.GetInputCMP().isKeyLPressed = true;
            });
        }
        else
        {
            em.forAll([](Entity& entity)
            {
                entity.GetInputCMP().isKeyLPressed = false;
            });
        }
    }

};
