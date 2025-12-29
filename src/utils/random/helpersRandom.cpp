#include "helpersRandom.h"


namespace RandomHelpers
{
    std::mt19937 &Random::GetEngine()
    {
        static thread_local std::mt19937 s_Engine{std::random_device{}()};
        return s_Engine;
    }

    void Random::Init()
    {
        GetEngine().seed(std::random_device{}());
    }

    float Random::Float()
    {
        static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
        return distribution(GetEngine());
    }

    float Random::Float(float min, float max)
    {
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(GetEngine());
    }

    int Random::Int(int min, int max)
    {
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(GetEngine());
    }

    bool Random::Bool(float probability)
    {
        std::bernoulli_distribution distribution(probability);
        return distribution(GetEngine());
    }

}