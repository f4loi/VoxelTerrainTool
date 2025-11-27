#pragma once
#include "Igraficos.h"
#include "raylib.h"

class MotorRaylib : public Igraficos
{
public:
    void Init(int screenW, int screenH, const char* title) override;
    bool WindowShouldClose() override;
    void BeginDrawing() override;
    void EndDrawing() override;
    void DrawSquare(int posX, int posY, int size, char r, char g, char b, char a) override;
};
