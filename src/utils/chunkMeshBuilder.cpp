#include "utils/ChunkMeshBuilder.h"
#include <raymath.h>
#include <cstring>

static bool IsTransparent(VoxelType type)
{
    return type == VoxelType::EMPTY || type == VoxelType::WATER;
}

Model ChunkMeshBuilder::BuildMesh(const ChunkCMP &chunk)
{
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned char> colors;

    // Colores según el tipo de Vóxel
    auto GetVoxelColor = [](VoxelType type) -> Color
    {
        switch (type)
        {
        case VoxelType::GRASS:
            return GREEN;
        case VoxelType::DIRT:
            return BROWN;
        case VoxelType::STONE:
            return GRAY;
        case VoxelType::WATER:
            return BLUE;
        default:
            return WHITE;
        }
    };

    auto Darken = [](Color c, float factor) -> Color
    {
        return Color{
            (unsigned char)(c.r * factor),
            (unsigned char)(c.g * factor),
            (unsigned char)(c.b * factor),
            c.a};
    };

    // Función auxiliar para añadir una cara cuadrada (2 triángulos = 6 vértices)
    auto AddFace = [&](Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4, Vector3 normal, Color c)
    {
        // Triángulo 1
        vertices.insert(vertices.end(), {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z});
        // Triángulo 2
        vertices.insert(vertices.end(), {p1.x, p1.y, p1.z, p3.x, p3.y, p3.z, p4.x, p4.y, p4.z});

        for (int i = 0; i < 6; i++)
        {
            normals.insert(normals.end(), {normal.x, normal.y, normal.z});
            colors.insert(colors.end(), {c.r, c.g, c.b, c.a});
        }
    };

    // --- RECORRIDO DEL CHUNK (FACE CULLING) ---
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {

                VoxelType current = chunk.GetVoxel(x, y, z);
                if (current == VoxelType::EMPTY)
                    continue;

                Color baseColor = GetVoxelColor(current);
                Vector3 pos = {(float)x, (float)y, (float)z};

                // 1. CARA ARRIBA (+Y): La luz da de lleno (100% de brillo)
                if (y == CHUNK_HEIGHT - 1 || IsTransparent(chunk.GetVoxel(x, y + 1, z)))
                {
                    AddFace({pos.x, pos.y + 1, pos.z + 1}, {pos.x + 1, pos.y + 1, pos.z + 1},
                            {pos.x + 1, pos.y + 1, pos.z}, {pos.x, pos.y + 1, pos.z},
                            {0, 1, 0}, baseColor);
                }

                // 2. CARA ABAJO (-Y): Es el suelo/techo de cuevas (sombra total, 40% de brillo)
                if (y == 0 || IsTransparent(chunk.GetVoxel(x, y - 1, z)))
                {
                    AddFace({pos.x, pos.y, pos.z}, {pos.x + 1, pos.y, pos.z},
                            {pos.x + 1, pos.y, pos.z + 1}, {pos.x, pos.y, pos.z + 1},
                            {0, -1, 0}, Darken(baseColor, 0.4f));
                }

                // 3. CARA DERECHA (+X): (Luz lateral, 80% de brillo)
                if (x == CHUNK_SIZE - 1 || IsTransparent(chunk.GetVoxel(x + 1, y, z)))
                {
                    AddFace({pos.x + 1, pos.y, pos.z}, {pos.x + 1, pos.y + 1, pos.z},
                            {pos.x + 1, pos.y + 1, pos.z + 1}, {pos.x + 1, pos.y, pos.z + 1},
                            {1, 0, 0}, Darken(baseColor, 0.8f));
                }

                // 4. CARA IZQUIERDA (-X): (Luz lateral, 80% de brillo)
                if (x == 0 || IsTransparent(chunk.GetVoxel(x - 1, y, z)))
                {
                    AddFace({pos.x, pos.y, pos.z + 1}, {pos.x, pos.y + 1, pos.z + 1},
                            {pos.x, pos.y + 1, pos.z}, {pos.x, pos.y, pos.z},
                            {-1, 0, 0}, Darken(baseColor, 0.8f));
                }

                // 5. CARA FRENTE (+Z): (Sombra frontal, 60% de brillo)
                if (z == CHUNK_SIZE - 1 || IsTransparent(chunk.GetVoxel(x, y, z + 1)))
                {
                    AddFace({pos.x + 1, pos.y, pos.z + 1}, {pos.x + 1, pos.y + 1, pos.z + 1},
                            {pos.x, pos.y + 1, pos.z + 1}, {pos.x, pos.y, pos.z + 1},
                            {0, 0, 1}, Darken(baseColor, 0.6f));
                }

                // 6. CARA ATRÁS (-Z): (Sombra trasera, 60% de brillo)
                if (z == 0 || IsTransparent(chunk.GetVoxel(x, y, z - 1)))
                {
                    AddFace({pos.x, pos.y, pos.z}, {pos.x, pos.y + 1, pos.z},
                            {pos.x + 1, pos.y + 1, pos.z}, {pos.x + 1, pos.y, pos.z},
                            {0, 0, -1}, Darken(baseColor, 0.6f));
                }
            }
        }
    }

    // --- CONSTRUIR MESH DE RAYLIB Y SUBIR A VRAM ---
    Mesh mesh = {0};
    mesh.vertexCount = (int)vertices.size() / 3;
    mesh.triangleCount = mesh.vertexCount / 3;

    mesh.vertices = (float *)RL_MALLOC(vertices.size() * sizeof(float));
    mesh.normals = (float *)RL_MALLOC(normals.size() * sizeof(float));
    mesh.colors = (unsigned char *)RL_MALLOC(colors.size() * sizeof(unsigned char));

    memcpy(mesh.vertices, vertices.data(), vertices.size() * sizeof(float));
    memcpy(mesh.normals, normals.data(), normals.size() * sizeof(float));
    memcpy(mesh.colors, colors.data(), colors.size() * sizeof(unsigned char));

    UploadMesh(&mesh, false); // ¡Envía los vértices a la VRAM de la gráfica de una pasada!

    Model model = LoadModelFromMesh(mesh);
    return model;
}