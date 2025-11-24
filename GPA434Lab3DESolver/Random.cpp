#include "Random.h"

#include <chrono>


#include <limits>


std::default_random_engine Random::smEngine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));


bool Random::event(double probability)
{
    return std::bernoulli_distribution(probability)(smEngine);
}

int Random::integer()
{
    return integer(std::numeric_limits<int>::max());
}

int Random::integer(int maximum)
{
    return integer(0, maximum);
}

int Random::integer(int minimum, int maximum)
{
    return std::uniform_int_distribution<int>(minimum, maximum)(smEngine);
}

size_t Random::size()
{
    return size(std::numeric_limits<int>::max());
}

size_t Random::size(size_t maximum)
{
    return size(0ull, maximum);
}

size_t Random::size(size_t minimum, size_t maximum)
{
    return std::uniform_int_distribution<size_t>(minimum, maximum)(smEngine);
}

double Random::real()
{
    return real(1.0);
}

double Random::real(double maximum)
{
    return real(0.0, maximum);
}

double Random::real(double minimum, double maximum)
{
    return std::uniform_real_distribution<double>(minimum, maximum)(smEngine);
}

