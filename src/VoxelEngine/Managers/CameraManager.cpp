#include "VoxelEngine/Managers/CameraManager.h"
#include <raymath.h>

void CameraManager::Init()
{
    camera.position = Vector3{-10.0f, 25.0f, -10.0f};
    camera.target = Vector3{8.0f, 0.0f, 8.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void CameraManager::Update(bool canUseCamera)
{
    // --- 1. ACTIVACIÓN INTELIGENTE DEL CURSOR ---
    // Solo empezamos a girar si haces clic Y estabas encima del panel 3D
    if (canUseCamera && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        isCameraActive = true;
        savedMousePos = GetMousePosition(); // Guardamos dónde hiciste clic
        DisableCursor();                    // Ocultamos el ratón
    }

    // Al soltar el botón, apagamos la cámara y devolvemos el ratón a su sitio
    if (isCameraActive && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
    {
        isCameraActive = false;
        EnableCursor();
        SetMousePosition((int)savedMousePos.x, (int)savedMousePos.y); // Teletransporte mágico
    }

    // SI LA CÁMARA NO ESTÁ ACTIVA, SALIMOS.
    // Esto evita que vueles con WASD mientras escribes un número en ImGui.
    if (!isCameraActive)
        return;

    // --- 2. ROTACIÓN (Mirar con el ratón) ---
    Vector3 rotation = {0.0f, 0.0f, 0.0f};
    Vector2 mouseDelta = GetMouseDelta();
    float turnSpeed = 0.05f;

    rotation.x = mouseDelta.x * turnSpeed;
    rotation.y = mouseDelta.y * turnSpeed;

    UpdateCameraPro(&camera, {0.0f, 0.0f, 0.0f}, rotation, 0.0f);

    // --- 3. CÁLCULO DE VECTORES 3D Y MOVIMIENTO SPECTATOR ---
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));

    float moveSpeed = 0.5f;
    if (IsKeyDown(KEY_LEFT_SHIFT))
        moveSpeed = 1.5f;

    Vector3 velocity = {0.0f, 0.0f, 0.0f};

    if (IsKeyDown(KEY_W))
        velocity = Vector3Add(velocity, Vector3Scale(forward, moveSpeed));
    if (IsKeyDown(KEY_S))
        velocity = Vector3Subtract(velocity, Vector3Scale(forward, moveSpeed));
    if (IsKeyDown(KEY_D))
        velocity = Vector3Add(velocity, Vector3Scale(right, moveSpeed));
    if (IsKeyDown(KEY_A))
        velocity = Vector3Subtract(velocity, Vector3Scale(right, moveSpeed));
    if (IsKeyDown(KEY_E))
        velocity.y += moveSpeed;
    if (IsKeyDown(KEY_Q))
        velocity.y -= moveSpeed;

    camera.position = Vector3Add(camera.position, velocity);
    camera.target = Vector3Add(camera.target, velocity);
}