#pragma once
#include "../CMP/RenderCMP.h"
#include "../CMP/PhysicCMP.h"

class Entity
{
private:
    int id;
    RenderCMP renderCMP;
    PhysicCMP physicCMP;

public:
    int GetId() const { return id; }
    void SetId(int newId) { id = newId; }
    RenderCMP& GetRenderCMP() { return renderCMP; }
    PhysicCMP& GetPhysicCMP() { return physicCMP; }
};