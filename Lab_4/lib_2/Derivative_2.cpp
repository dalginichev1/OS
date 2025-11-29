#include "Derivative_2.hpp"

#include <cmath>

float Derivative2::Derivative(float A, float deltaX)
{
    return (std::cos(A + deltaX) - std::cos(A - deltaX)) / (2.0f * deltaX);
}