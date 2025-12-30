#pragma once
#include "utils/random/helpersRandom.h"


struct PhysicCMP
{
    float x{200.0f};
    float y{300.0f};

    float vx {RandomHelpers::Random::Float(1.0f, 5.0f)};
};


//Crear utils con helpers rand global
//Cambiar a llaves