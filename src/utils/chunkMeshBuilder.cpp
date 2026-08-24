#include "utils/ChunkMeshBuilder.h"
#include <raymath.h>
#include <cstring>

/*
    ===================================================================================================
                                            Auxiliary functions
    ===================================================================================================
*/
namespace
{

    /*
    Function: IsTransparent
    Description: Determines if a given voxel type is considered transparent (does not block light).
    Parameters:
        - VoxelType type: The type of voxel to check.
    Returns:
        - bool: True if the voxel type is transparent (EMPTY or WATER), if not false.
    */
    inline bool IsTransparent(VoxelType type)
    {
        return type == VoxelType::EMPTY || type == VoxelType::WATER;
    }

    /*
        Function: GetVoxelColor
        Description: Returns the color associated with a given voxel type.
        Parameters:
            - VoxelType type: The type of voxel for which to get the color.
        Returns:
            - Color: The color corresponding to the voxel type.
    */
    inline Color GetVoxelColor(VoxelType type)
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
        case VoxelType::SAND:
            return Color{238, 214, 175, 255};
        case VoxelType::SNOW:
            return WHITE;
        default:
            return WHITE;
        }
    }

    /*
        Function: Darken
        Description: Darkens a given color by a specified factor.
        Parameters:
            - Color c: The original color.
            - float factor: The factor by which to darken the color.
        Returns:
            - Color: The darkened color.
    */
    inline auto Darken = [](Color c, float factor) -> Color
    {
        return Color{
            (unsigned char)(c.r * factor),
            (unsigned char)(c.g * factor),
            (unsigned char)(c.b * factor),
            c.a};
    };
}

/*
    Function: BuildMesh
    Description: Constructs a 3D mesh from the voxel data of a given chunk, applying face culling and lighting effects.
    Parameters:
        - const ChunkCMP& chunk: The chunk data containing voxel information.
    Returns:
        - Model: A raylib Model object representing the generated mesh for the chunk.
*/
Model ChunkMeshBuilder::BuildMesh(const ChunkCMP &chunk)
{
    std::vector<float> vertices;       // Vector to hold vertex positions (x, y, z)
    std::vector<float> normals;        // Vector to hold normal vectors for lighting calculations (x, y, z)
    std::vector<unsigned char> colors; // Vector to hold vertex colors (r, g, b, a)

    /*
        Function: AddFace
        Description: Adds a face to the mesh with the specified vertices, normal, and color.
        Parameters:
            - Vector3 p1, p2, p3, p4: The four vertices of the face.
            - Vector3 normal: The normal vector for lighting calculations.
            - Color c: The color for the face.
    */
    auto AddFace = [&](Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4, Vector3 normal, Color c)
    {
        vertices.insert(vertices.end(), {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z});

        vertices.insert(vertices.end(), {p1.x, p1.y, p1.z, p3.x, p3.y, p3.z, p4.x, p4.y, p4.z});

        for (int i = 0; i < 6; i++)
        {
            normals.insert(normals.end(), {normal.x, normal.y, normal.z});
            colors.insert(colors.end(), {c.r, c.g, c.b, c.a});
        }
    };

    // =====================================================================
    //  Chunk mesh generation loop: Iterate through each voxel in the chunk and generate faces for visible voxels.
    // =====================================================================
    for (int y = 0; y < CHUNK_HEIGHT; y++) // first height Y
    {
        for (int z = 0; z < CHUNK_SIZE; z++) // second depth Z
        {
            for (int x = 0; x < CHUNK_SIZE; x++) // third width X
            {
                // Get the current voxel type at the specified coordinates
                VoxelType current = chunk.GetVoxel(x, y, z);
                // if current is empty (air, void), skip to the next voxel
                if (current == VoxelType::EMPTY)
                {
                    continue;
                }
                // Get the base color for the current voxel type and its position in 3D space
                Color baseColor = GetVoxelColor(current);
                Vector3 pos = {(float)x, (float)y, (float)z};

                /*
                ===============================
                Face culling and fake lighting
                ===============================
                there are some general rules for face culling and lighting:
                1. One face  of the cube is drawn only if:
                      - The voxel is at the edge of the chunk (x, y, z == 0 or CHUNK_SIZE-1)
                      - The adjacent voxel in that direction is transparent (EMPTY or WATER)
                2. for eacg face of the cube, we apply a different brightness factor to simulate lighting:
                      - Top face (+Y): full brightness (100%)
                      - Bottom face (-Y): shadowed (40%)
                      - Side faces (+X, -X, +Z, -Z): lateral light (80% or 60% depending on the face)

                */

                // Face 1: Up (+Y): It's the top of the terrain (full brightness, 100%)
                if (y == CHUNK_HEIGHT - 1 || IsTransparent(chunk.GetVoxel(x, y + 1, z)))
                {
                    AddFace({pos.x, pos.y + 1, pos.z + 1}, {pos.x + 1, pos.y + 1, pos.z + 1},
                            {pos.x + 1, pos.y + 1, pos.z}, {pos.x, pos.y + 1, pos.z},
                            {0, 1, 0}, baseColor);
                }

                // Face 2: Down (-Y): It's the bottom of the terrain (shadowed, 40% brightness)
                if (y == 0 || IsTransparent(chunk.GetVoxel(x, y - 1, z)))
                {
                    AddFace({pos.x, pos.y, pos.z}, {pos.x + 1, pos.y, pos.z},
                            {pos.x + 1, pos.y, pos.z + 1}, {pos.x, pos.y, pos.z + 1},
                            {0, -1, 0}, Darken(baseColor, 0.4f));
                }

                // Face 3: Right (+X): It's the right side of the terrain (lateral light, 80% brightness)
                if (x == CHUNK_SIZE - 1 || IsTransparent(chunk.GetVoxel(x + 1, y, z)))
                {
                    AddFace({pos.x + 1, pos.y, pos.z}, {pos.x + 1, pos.y + 1, pos.z},
                            {pos.x + 1, pos.y + 1, pos.z + 1}, {pos.x + 1, pos.y, pos.z + 1},
                            {1, 0, 0}, Darken(baseColor, 0.8f));
                }

                // Face 4: Left (-X): It's the left side of the terrain (lateral light, 80% brightness)
                if (x == 0 || IsTransparent(chunk.GetVoxel(x - 1, y, z)))
                {
                    AddFace({pos.x, pos.y, pos.z + 1}, {pos.x, pos.y + 1, pos.z + 1},
                            {pos.x, pos.y + 1, pos.z}, {pos.x, pos.y, pos.z},
                            {-1, 0, 0}, Darken(baseColor, 0.8f));
                }

                // Face 5: Front (+Z): It's the front side of the terrain (lateral light, 60% brightness)
                if (z == CHUNK_SIZE - 1 || IsTransparent(chunk.GetVoxel(x, y, z + 1)))
                {
                    AddFace({pos.x + 1, pos.y, pos.z + 1}, {pos.x + 1, pos.y + 1, pos.z + 1},
                            {pos.x, pos.y + 1, pos.z + 1}, {pos.x, pos.y, pos.z + 1},
                            {0, 0, 1}, Darken(baseColor, 0.6f));
                }

                // Face 6: Back (-Z): It's the back side of the terrain (shadowed, 60% brightness)
                if (z == 0 || IsTransparent(chunk.GetVoxel(x, y, z - 1)))
                {
                    AddFace({pos.x, pos.y, pos.z}, {pos.x, pos.y + 1, pos.z},
                            {pos.x + 1, pos.y + 1, pos.z}, {pos.x + 1, pos.y, pos.z},
                            {0, 0, -1}, Darken(baseColor, 0.6f));
                }
            }
        }
    }

    // =====================================================================
    // BUILD RAYLIB MESH AND UPLOAD TO VRAM
    // =====================================================================

    // Initialize an empty Raylib mesh structure
    Mesh mesh = {0};
    // Each vertex has 3 components (x, y, z), so the total number of vertices is the size of the vertices vector divided by 3
    mesh.vertexCount = (int)vertices.size() / 3;
    // Each triangle consists of 3 vertices, so the total number of triangles is the number of vertices divided by 3
    mesh.triangleCount = mesh.vertexCount / 3;

    // Allocate memory for the mesh's vertex attributes (positions, normals, colors) and copy the data from the vectors
    // we use RL_MALLOC to allocate memory for the mesh's vertex attributes, which is a raylib function that wraps standard memory allocation functions and ensures proper alignment for GPU usage.
    mesh.vertices = (float *)RL_MALLOC(vertices.size() * sizeof(float));
    mesh.normals = (float *)RL_MALLOC(normals.size() * sizeof(float));
    mesh.colors = (unsigned char *)RL_MALLOC(colors.size() * sizeof(unsigned char));

    // Copy the data from the vectors to the allocated memory for the mesh's vertex attributes
    // we use memcpy to copy the data from the vectors to the allocated memory for the mesh's vertex attributes.
    // This is a standard C function that copies a specified number of bytes from one memory location to another.
    memcpy(mesh.vertices, vertices.data(), vertices.size() * sizeof(float));
    memcpy(mesh.normals, normals.data(), normals.size() * sizeof(float));
    memcpy(mesh.colors, colors.data(), colors.size() * sizeof(unsigned char));

    // Upload the mesh data to the GPU (VRAM) for rendering. The second parameter 'false' indicates that the mesh is not dynamic and will not change frequently.
    UploadMesh(&mesh, false);

    // Load the mesh into a raylib Model structure, which can be used for rendering in the game engine
    Model model = LoadModelFromMesh(mesh);
    return model;
}