#include "Derivative_1.hpp"

#include <cmath>

float Derivative1::Derivative(float A, float deltaX)
{
    return (std::cos(A + deltaX) - std::cos(A)) / deltaX;
}