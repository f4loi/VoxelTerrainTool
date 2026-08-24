#include "VoxelEngine/Managers/CameraManager.h"
#include <raymath.h>

/*
    Function: Init
    Description: Initializes the camera with default position, target, up vector, field of view, and projection type.
*/
void CameraManager::Init()
{
    camera.position = Vector3{-10.0f, 25.0f, -10.0f};
    camera.target = Vector3{8.0f, 0.0f, 8.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

/*
    Function: Update
    Description: Updates the camera's position and orientation based on user input (mouse and keyboard).
    Parameters:
        - bool canUseCamera: Indicates whether the camera can be controlled by the user.
*/
void CameraManager::Update(bool canUseCamera)
{
    // Only activates the camera rotation if the user clicks the right mouse button and the camera is allowed to be used.
    if (canUseCamera && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        // Activate camera control mode
        isCameraActive = true;
        // Save the current mouse position to restore it later when the camera control is deactivated
        savedMousePos = GetMousePosition();
        DisableCursor();
    }

    // Deactivates the camera control mode if the right mouse button is released, restoring the cursor and its position.
    if (isCameraActive && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
    {
        isCameraActive = false;
        EnableCursor();
        // Restore the mouse position to where it was before camera control was activated
        SetMousePosition((int)savedMousePos.x, (int)savedMousePos.y);
    }

    // IF THE CAMERA IS NOT ACTIVE, EXIT EARLY.
    // This prevents the player from flying around with WASD while typing values in the ImGui panels.
    if (!isCameraActive)
    {
        return;
    }

    // Rotation calculations based on mouse movement, applying a turn speed factor to control sensitivity.
    Vector3 rotation = {0.0f, 0.0f, 0.0f};
    // Get the mouse movement delta since the last frame
    Vector2 mouseDelta = GetMouseDelta();
    // Define a turn speed factor to control how fast the camera rotates based on mouse movement
    float turnSpeed = 0.05f;

    rotation.x = mouseDelta.x * turnSpeed;
    rotation.y = mouseDelta.y * turnSpeed;

    // Update the camera's position and orientation based on the calculated rotation, without changing the zoom level.
    UpdateCameraPro(&camera, {0.0f, 0.0f, 0.0f}, rotation, 0.0f);

    // Movement calculations based on keyboard input (WASD for forward/backward and strafing, Q/E for vertical movement).
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));

    float moveSpeed = 0.5f;
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        moveSpeed = 1.5f;
    }

    Vector3 velocity = {0.0f, 0.0f, 0.0f};

    // Apply directional movement based on WASD keys
    if (IsKeyDown(KEY_W))
    {
        velocity = Vector3Add(velocity, Vector3Scale(forward, moveSpeed));
    }
    if (IsKeyDown(KEY_S))
    {
        velocity = Vector3Subtract(velocity, Vector3Scale(forward, moveSpeed));
    }

    if (IsKeyDown(KEY_D))
    {
        velocity = Vector3Add(velocity, Vector3Scale(right, moveSpeed));
    }

    if (IsKeyDown(KEY_A))
    {
        velocity = Vector3Subtract(velocity, Vector3Scale(right, moveSpeed));
    }

    // Vertical movement (E to go up, Q to go down)
    if (IsKeyDown(KEY_E))
    {
        velocity.y += moveSpeed;
    }

    if (IsKeyDown(KEY_Q))
    {
        velocity.y -= moveSpeed;
    }

    // Update the camera's position and target based on the calculated velocity, allowing for smooth movement in 3D space.
    camera.position = Vector3Add(camera.position, velocity);
    camera.target = Vector3Add(camera.target, velocity);
}