#ifndef PiecewiseConstant1D
#define PiecewiseConstant1D
#include "Math.glsl"
float PiecewiseConstant1D_Sample(int o, float u, int func_size, float min, float max, float cdf0, float cdf1, float funInt, float func, inout float pdf)
{
    float du = u - cdf0;
    if (cdf1 - cdf0 > 0)
        du /= cdf1 - cdf0;
    pdf = (funInt > 0) ? func / funInt : 0;
    return Lerp((o + du) / func_size, min, max);
}

#endif