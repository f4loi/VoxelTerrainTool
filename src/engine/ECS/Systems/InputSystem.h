#pragma once
#include <raylib.h>
#include "../../ECS/Managers/EntityManager.h"

class InputSystem
{
private:
public:
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
