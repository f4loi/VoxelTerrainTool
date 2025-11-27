#pragma once

class Igraficos
{
private:
public:
    virtual ~Igraficos() = default;

    virtual void Init(int screenW, int screenH, const char* title) = 0;
    virtual bool WindowClose() = 0;
    virtual void EmpezarDrawing() = 0;
    virtual void TerminarDrawing() = 0;
    virtual void DrawRectangulo(int posX, int posY, int size, char r, char g, char b, char a) = 0;
};

