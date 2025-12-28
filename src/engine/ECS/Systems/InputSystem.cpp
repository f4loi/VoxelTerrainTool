#include "InputSystem.h"

void InputSystem::Update(EntityManager &em)
{

    em.forAll(&InputSystem::UpdateOneEntity);
}

void InputSystem::UpdateOneEntity(Entity ent)
{

    EntityManager *manager = ent.getParent();

    int id = ent.getNextId();

    if (manager != nullptr)
    {

        InputCMP *input = manager->GetInput(id);

        if (input != nullptr)
        {

            input->isKeyLPressed = IsKeyDown(KEY_L);
        }
    }
}