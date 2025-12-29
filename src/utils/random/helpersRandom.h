#include <random>
#pragma once

#include <random>
#include <ctime>

namespace RandomHelpers
{

    class Random
    {
    public:
        static void Init();

        static float Float();

        static float Float(float min, float max);

        static int Int(int min, int max);

        static bool Bool(float probability = 0.5f);

    private:
        static std::mt19937 &GetEngine();
    };

}