#pragma once

class AbsDerivative
{
    public:
        virtual float Derivative(float A, float deltaX) = 0;
        virtual ~AbsDerivative() = default;
};