#pragma once
#include "Igraficos.h"
#include "raylib.h"

class MotorRaylib : public Igraficos
{
public:
    MotorRaylib() = default;
    ~MotorRaylib();
    void Init(int screenW, int screenH, const char* title) override;
    bool WindowClose() override;
    void EmpezarDrawing() override;
    void TerminarDrawing() override;
    void DrawRectangulo(int posX, int posY, int size, char r, char g, char b, char a) override;
};
