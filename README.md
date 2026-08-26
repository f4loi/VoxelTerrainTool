<div align="center">

# Voxel Terrain Generator & Editor

**A high-performance procedural voxel terrain generator and interactive 3D sculpting tool.**

[![C++](https://img.shields.io/badge/C++-17+-blue.svg?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![Raylib](https://img.shields.io/badge/Raylib-5.0-red.svg?style=for-the-badge&logo=raylib)](https://www.raylib.com/)
[![Dear ImGui](https://img.shields.io/badge/Dear_ImGui-Docking-darkblue.svg?style=for-the-badge)](https://github.com/ocornut/imgui)

</div>

## About The Project

This project is a standalone **Voxel Engine and Terrain Editor** built from scratch in C++. It allows users to procedurally generate infinite 3D worlds using Perlin noise and seamlessly jump in to hand-sculpt the terrain using a suite of professional geological brush tools. 

Designed with a custom **Entity-Component-System (ECS)** architecture, the engine handles real-time chunk mesh generation, face culling, and off-screen rendering to provide a fluid editing experience. 

Whether you want to generate rolling hills, carve rivers, or raise jagged mountain peaks, this editor gives you the power to shape the world and then **export your creation directly to Unreal Engine 5**.

---

## Key Features

*  **Procedural Generation**: Infinite terrain generation powered by layered Perlin noise with adjustable seeds, scale, and water levels. Automatically generates biomes (Oceans, Coastlines, Meadows, Mountains).
*  **Advanced Sculpting Tools**: 
    *   **Brushes**: Raise, Lower, Flatten, Smooth, Roughen, Terrace, and Sharpen.
    *   **Shapes**: Soft Circle, Hard Circle, Square, and Noise (Splatter).
*  **Dual-Viewport Editing**: 
    *   **2D Map**: A top-down interactive heightmap and biome view where you can paint directly using raycasted world coordinates.
    *   **3D Viewport**: An off-screen rendered 3D preview of the voxel world with free-cam controls.
*  **Non-Destructive Workflow**: Built-in Undo system (`Ctrl+Z`) that takes memory-efficient snapshots of the chunk topology.
*  **Persistence & Export**: Save/Load your editor state via JSON, and export the entire voxel world (Run-Length Encoded) to an Unreal Engine 5 compatible JSON format.
*  **Highly Optimized**: Features custom chunk mesh building with hidden face culling and ambient occlusion / fake lighting.

---

## Showcase



### Interactive Sculpting & UI
<div align="center">
  <img src="src/sources/gif1.gif" alt="Editor UI & Sculpting" width="800"/>
  <p><i>The dual-viewport interface. Painting on the 2D map instantly rebuilds the 3D voxel meshes.</i></p>
</div>

### Procedural Biomes
<div align="center">
  <img src="src/sources/gif2.gif" alt="Procedural Biomes" width="800"/>
  <p><i>Seamless transitions between Deep Oceans, Coastlines, Meadows, and Snow-capped Mountains.</i></p>
</div>

### Geological Tools (Terrace & Roughen)
<div align="center">
  <img src="src/sources/gif3.gif" alt="Advanced Brushes" width="800"/>
  <p><i>Using the Terrace brush to create natural geological steps and the Roughen tool for jagged rocks.</i></p>
</div>

---

## Technology Stack & Architecture

*   **Language**: `C++`
*   **Rendering Framework**: `Raylib` (Low-level OpenGL abstraction)
*   **Graphical User Interface**: `Dear ImGui` (integrated via rlImGui)
*   **Serialization**: `nlohmann/json` for config persistence and UE5 export.
*   **Architecture**: Custom-built **Entity Component System (ECS)** utilizing `Slotmaps` for contiguous memory access and Cache-friendly entity management. Systems are decoupled into `RenderSystemMeta`, `PhysicsSystemMeta`, and `TerrainSystem`.

---

## Controls & Shortcuts

### 3D Viewport
| Action | Key/Input |
| :--- | :--- |
| **Rotate Camera** | `Right Click + Drag` |
| **Move** | `W` `A` `S` `D` |
| **Up / Down** | `E` / `Q` |
| **Sprint** | `Shift` |

### Editor & Sculpting
| Action | Key/Input |
| :--- | :--- |
| **Increase / Decrease Brush Size** | `+` / `-` |
| **Increase / Decrease Strength** | `Up Arrow` / `Down Arrow` |
| **Undo Last Stroke** | `Ctrl + Z` |

---

## Play It Now

You don't need to compile the code to try the editor! A pre-compiled executable is available for Windows.

1. Go to the [Releases page](../../releases/latest).
2. Download the `VoxelTerrainTool.zip` file.
3. Extract the contents to any folder.
4. Run `VoxelEngine.exe` and start sculpting!

---

<div align="center">
  <i>If you found this project interesting, please consider leaving a ⭐ on the repository!</i>
</div>
