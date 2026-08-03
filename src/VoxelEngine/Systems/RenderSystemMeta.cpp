#include "VoxelEngine/Systems/RenderSystemMeta.h"

void RenderSystemMeta::Init()
{
    camera.position = Vector3{-10.0f, 25.0f, -10.0f};
    camera.target = Vector3{8.0f, 0.0f, 8.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void RenderSystemMeta::Update(EntityManagerMeta &em)
{
    // --- 1. TRASLACIÓN (Moverse) ---
    // Lo sacamos fuera del clic para que WASD funcione SIEMPRE
    float moveSpeed = 0.5f;
    Vector3 movement = {0.0f, 0.0f, 0.0f};

    if (IsKeyDown(KEY_W))
        movement.x += moveSpeed; // Adelante
    if (IsKeyDown(KEY_S))
        movement.x -= moveSpeed; // Atrás
    if (IsKeyDown(KEY_D))
        movement.y += moveSpeed; // Derecha
    if (IsKeyDown(KEY_A))
        movement.y -= moveSpeed; // Izquierda
    if (IsKeyDown(KEY_E))
        movement.z += moveSpeed; // Subir
    if (IsKeyDown(KEY_Q))
        movement.z -= moveSpeed; // Bajar

    // --- 2. GESTIÓN DEL CURSOR ---
    // Solo lo bloqueamos en el instante que haces clic, y lo liberamos al soltar
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        DisableCursor();
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
        EnableCursor();

    // --- 3. ROTACIÓN (Mirar) ---
    Vector3 rotation = {0.0f, 0.0f, 0.0f};

    // Solo giramos la cámara si el clic derecho se mantiene presionado
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Vector2 mouseDelta = GetMouseDelta();
        float turnSpeed = 0.05f;

        rotation.x = mouseDelta.x * turnSpeed; // Mirar Izquierda/Derecha
        rotation.y = mouseDelta.y * turnSpeed; // Mirar Arriba/Abajo
    }

    // --- 4. ACTUALIZAR CÁMARA ---
    UpdateCameraPro(&camera, movement, rotation, 0.0f);

    // --- RENDERIZADO DEL MUNDO 3D ---
    BeginMode3D(camera);
    em.forAll(&RenderSystemMeta::UpdateOneEntity);
    EndMode3D();
}

// ¡ESTA ES LA FUNCIÓN QUE FALTABA!
void RenderSystemMeta::UpdateOneEntity(EntityMeta ent)
{
    EntityManagerMeta *manager{ent.getParent()};
    uint16_t id{ent.getNextId()};

    if (manager != nullptr)
    {
        ChunkCMP *chunk = manager->getComponent<ChunkCMP>(id);
        RenderCMP *render = manager->getComponent<RenderCMP>(id);

        if (chunk != nullptr && render != nullptr && render->isRendered)
        {
            float startX = chunk->chunkX * CHUNK_SIZE;
            float startZ = chunk->chunkZ * CHUNK_SIZE;

            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    for (int x = 0; x < CHUNK_SIZE; x++)
                    {

                        VoxelType type = chunk->GetVoxel(x, y, z);
                        if (type != VoxelType::EMPTY)
                        {
                            Vector3 pos = {startX + x, (float)y, startZ + z};
                            Color color = WHITE;

                            switch (type)
                            {
                            case VoxelType::DIRT:
                                color = BROWN;
                                break;
                            case VoxelType::GRASS:
                                color = GREEN;
                                break;
                            case VoxelType::WATER:
                                color = BLUE;
                                break;
                            case VoxelType::STONE:
                                color = GRAY;
                                break;
                            default:
                                break;
                            }

                            // Dibujamos el bloque y sus bordes
                            DrawCube(pos, 1.0f, 1.0f, 1.0f, color);
                            DrawCubeWires(pos, 1.0f, 1.0f, 1.0f, BLACK);
                        }
                    }
                }
            }
        }
    }
}

bool RenderSystemMeta::WindowShouldClose()
{
    return ::WindowShouldClose();
}