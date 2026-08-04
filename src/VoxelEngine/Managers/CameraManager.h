#pragma once
#include <raylib.h>

class CameraManager {
private:
    Camera3D camera = { 0 };
    bool isCameraActive = false;
    Vector2 savedMousePos = { 0.0f, 0.0f };

public:
    void Init();
    void Update(bool canUseCamera);
    
    const Camera3D& GetCamera() const { return camera; } 
};