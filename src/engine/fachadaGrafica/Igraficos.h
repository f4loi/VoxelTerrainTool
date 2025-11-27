#pragma once

class Igraficos
{
private:
public:
    virtual ~Igraficos() = default;

    virtual void Init(int screenW, int screenH, const char* title) = 0;
    virtual bool WindowShouldClose() = 0;
    virtual void BeginDrawing() = 0;
    virtual void EndDrawing() = 0;
    virtual void DrawSquare(int posX, int posY, int size, char r, char g, char b, char a) = 0;
};

