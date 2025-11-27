#pragma once

class Entity
{
private:
    int id;
public:
    int GetId() const { return id; }
    void SetId(int newId) { id = newId; }
};