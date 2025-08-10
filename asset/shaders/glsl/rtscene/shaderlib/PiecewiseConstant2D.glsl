#ifndef PiecewiseConstant2D
#define PiecewiseConstant2D 
#include "PiecewiseConstant1D.glsl"
#include "HDRParams.glsl"
/*
    // float func[h][w + 1], cdf[h][w + 1]
    // R: func   G: cdf
    texture2d ConditionalRG; 
    // vec4 Conditionaldata[h]
    // R: min   G: max B: funcInt  A: func_size = w
    texture1d Conditionaldata;
    // float func[h + 1], cdf[h + 1]
    // R: func   G: cdf
    texture1d MarginalRG;
    // R: min   G: max B: funcInt  A: func_size = h
    vec4 Marginaldata;
    // (w, h, min, max)
    vec4 Bound;  
*/ 

//#if HDR_FORMAT == USE_EXR
	layout(binding = 9, set = 0, rg32f) uniform image2D ConditionalRG;
	layout(binding = 10, set = 0, rgba32f) uniform image1D ConditionalDataRGBA;
	layout(binding = 11, set = 0, rg32f) uniform image1D MarginalRG; 
	layout(binding = 12, set = 0) uniform Params { HDRParams Hdr; }; 
//#endif

float GetMarginalFunc(int o)
{
    return imageLoad(MarginalRG, o).r;
}

float GetMarginalCDF(int o)
{
    return imageLoad(MarginalRG, o).g;
}

float GetMarginalMin()
{
    return Hdr.MarginalData.r;
}

float GetMarginalMax()
{
    return Hdr.MarginalData.g;
}

float GetMarginalFuncInt()
{
    return Hdr.MarginalData.b;
}

int GetMarginalFuncSize()
{
    return int(Hdr.MarginalData.a);
}

float GetConditionalFunc(int row, int col)
{
    return imageLoad(ConditionalRG, ivec2(row, col)).r;
}

float GetConditionalCDF(int row, int col)
{
    return imageLoad(ConditionalRG, ivec2(row, col)).g;
}

float GetConditionalMin(int row)
{
    return imageLoad(ConditionalDataRGBA, row).r;
}

float GetConditionalMax(int row)
{
    return imageLoad(ConditionalDataRGBA, row).g;
}

float GetConditionalFuncInt(int row)
{
    return imageLoad(ConditionalDataRGBA, row).b;
}

float GetConditionalFuncSize(int row)
{
    return imageLoad(ConditionalDataRGBA, row).a;
}



int FindConditionalInterval(int sz, float u, int row)
{
    int size = sz - 2;
    int first = 1;
    while (size > 0) {
        // Evaluate predicate at midpoint and update _first_ and _size_
        int half_ = size >> 1, middle = first + half_;
        bool predResult = GetConditionalCDF(row, middle) < u;
        first = predResult ? middle + 1 : first;
        size = predResult ? size - (half_ + 1) : half_;
    }
    return clamp(first - 1, 0, sz - 2);
}

int FindMarginalInterval(int sz, float u)
{
    int size = sz - 2;
    int first = 1;
    while (size > 0) {
        // Evaluate predicate at midpoint and update _first_ and _size_
        int half_ = size >> 1, middle = first + half_;
        bool predResult = GetMarginalCDF(middle) < u;
        first = predResult ? middle + 1 : first;
        size = predResult ? size - (half_ + 1) : half_;
    }
    return clamp(first - 1, 0, sz - 2);
}

float PiecewiseConstant2D_Conditional_Sample(float u, int func_size, float min, float max, inout float pdf, inout int row, inout int col)
{
    int cdf_size = func_size + 1;
    col = FindConditionalInterval(cdf_size, u, row);
    float cdf1 = GetConditionalCDF(row, col + 1);
    float cdf0 = GetConditionalCDF(row, col);
    float funInt = GetConditionalFuncInt(row);
    float func = GetConditionalFunc(row, col);
    float du = u - cdf0;
    return PiecewiseConstant1D_Sample(col, u, func_size, min, max, cdf0, cdf1, funInt, func, pdf);
}

float PiecewiseConstant2D_Marginal_Sample(float u, int func_size, float min, float max, inout float pdf, inout int row)
{
    int cdf_size = func_size + 1;
    row = FindMarginalInterval(cdf_size, u);
    float cdf1 = GetMarginalCDF(row + 1);
    float cdf0 = GetMarginalCDF(row);
    float funInt = GetMarginalFuncInt();
    float func = GetMarginalFunc(row);
    float du = u - cdf0;
    return PiecewiseConstant1D_Sample(row, u, func_size, min, max, cdf0, cdf1, funInt, func, pdf);
}

vec2 PiecewiseConstant2D_Sample(vec2 u, inout float pdf)
{
    int row = 0;
    int col = 0;
    float min = 0.f; 
    float max = 1.f;
    float pdfs[2];
    int MarginalFuncSize = GetMarginalFuncSize();
    // 采样行
    float d1 = PiecewiseConstant2D_Marginal_Sample(u.y, MarginalFuncSize, min, max, pdfs[1], row);
    int ConditionalFuncSize = int(GetConditionalFuncSize(row));
    // 采样列
    float d0 = PiecewiseConstant2D_Conditional_Sample(u.x, ConditionalFuncSize, min, max, pdfs[0], row, col);
    // 计算pdf
    pdf = pdfs[0] * pdfs[1];
    return vec2(d0, d1);
}

#endif 