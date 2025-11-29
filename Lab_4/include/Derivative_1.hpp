#pragma once

#include "AbsDerivative.hpp"

class Derivative1: public AbsDerivative
{
    float Derivative(float A, float deltaX) override;
    ~Derivative1() override = default;
};
