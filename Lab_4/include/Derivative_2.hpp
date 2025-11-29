#pragma once

#include "AbsDerivative.hpp"

class Derivative2: public AbsDerivative
{
    float Derivative(float A, float deltaX) override;
    ~Derivative2() override = default;
};