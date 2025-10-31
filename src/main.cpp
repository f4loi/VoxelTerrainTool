#include "raylib.h"

int main() {
    InitWindow(800, 600, "TestEngine");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello from TestEngine!", 200, 300, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
