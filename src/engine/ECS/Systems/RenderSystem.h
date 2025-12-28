/**
 * @file RenderSystem.h
 * @brief Sistema de render.
 * @date 22-12-2025
 */

#pragma once
#include "engine/IGrafica/MotorRaylib.h"
#include "engine/ECS/Managers/EntityManager.h"

/**
 * @class RenderSystem
 * @brief Sistema encargado de dibujar las entidades en pantalla.
 *
 * Este sistema itera sobre las entidades que tienen componentes de renderizado
 * y utiliza la fachada MotorRaylib para dibujarlas en la posición correcta.
 * Se encarga de abrir la ventana y gestionar el bucle de dibujo (BeginDraw/EndDraw).
 */
class RenderSystem
{
private:
    /**
     * @brief Instancia de la fachada gráfica.
     * Encapsula las llamadas de bajo nivel a la librería Raylib.
     */
    static MotorRaylib motorRaylib;
    
public:
   /**
     * @brief Inicializa el contexto gráfico.
     */
    void Init();

    /**
     * @brief Prepara el frame y llama al forAll.
     */
    void Update(EntityManager& em);

    /**
     * @brief Función estática que procesa UNA entidad.
     * Compatible con em.forAll.
     */
    static void UpdateOneEntity(Entity ent);
};