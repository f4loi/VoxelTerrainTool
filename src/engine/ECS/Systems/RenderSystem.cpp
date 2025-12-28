#include "RenderSystem.h"

// 1. IMPORTANTE: Definición de la variable estática fuera de la clase
MotorRaylib RenderSystem::motorRaylib;

void RenderSystem::Init()
{
    // Accedemos a la variable estática
    motorRaylib.Init(800, 600, "Engine Portfolio");
    SetTargetFPS(60);
}

void RenderSystem::Update(EntityManager &em)
{
    // 1. Preparar el frame (Inicio del dibujado)
    motorRaylib.EmpezarDrawing();
    motorRaylib.borrarBackground(0, 0, 0, 255);

    // 2. Ejecutar el bucle forAll pasando la función estática
    em.forAll(&RenderSystem::UpdateOneEntity);

    // 3. Finalizar el frame (Fin del dibujado)
    motorRaylib.TerminarDrawing();
}

void RenderSystem::UpdateOneEntity(Entity ent)
{
    // 1. Recuperar contexto y ID
    EntityManager* manager = ent.getParent();
    int id = ent.getNextId(); 

    if (manager != nullptr)
    {
        // 2. Recuperar componentes
        RenderCMP* render = manager->GetRender(id);
        PhysicCMP* physic = manager->GetPhysic(id);

        // 3. Verificar que existen y que la bandera de renderizado está activa
        if (render != nullptr && physic != nullptr && render->isRendered)
        {
            // 4. Dibujar usando la variable estática 'motorRaylib'
            // Nota: Asegúrate de que los tipos coincidan (int vs float)
            motorRaylib.DrawRectangulo(
                static_cast<int>(physic->x), 
                static_cast<int>(physic->y), 
                15, 
                255, 
                0, 0, 255
            );
        }
    }
}