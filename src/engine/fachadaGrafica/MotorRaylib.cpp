#include "MotorRaylib.h"

void MotorRaylib::Init(int screenW, int screenH, const char* title) {
    InitWindow(screenW, screenH, title);
}

bool MotorRaylib::WindowShouldClose() {
    return WindowShouldClose();
}

void MotorRaylib::BeginDrawing() {
    BeginDrawing();
}

void MotorRaylib::EndDrawing() {
    EndDrawing();
}

void MotorRaylib::DrawSquare(int posX, int posY, int size, char r, char g, char b, char a) {
    DrawRectangle(posX, posY, size, size, Color{static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b), static_cast<unsigned char>(a)});
}
