#ifndef TrowbridgeReitzDistribution_H
#define TrowbridgeReitzDistribution_H
#include "float.glsl"
#include "sampling.glsl"

struct TrowbridgeReitzDistribution
{
     float alpha_x;
     float alpha_y;
};

bool TrowbridgeReitzDistribution_EffectivelySmooth(TrowbridgeReitzDistribution d) 
{
    return max(d.alpha_x, d.alpha_y) < 1e-3f; 
}

TrowbridgeReitzDistribution TrowbridgeReitzDistribution_Init(float ax, float ay){
    TrowbridgeReitzDistribution d;
    d.alpha_x = ax;
    d.alpha_y = ay;
    if(!TrowbridgeReitzDistribution_EffectivelySmooth(d)){
        d.alpha_x = max(d.alpha_x,1e-4f);
        d.alpha_y = max(d.alpha_y,1e-4f);
    }
    return d;
} 

float TrowbridgeReitzDistribution_D(TrowbridgeReitzDistribution d, vec3 wm)
{
    float tan2Theta = Tan2Theta(wm);
    if (IsInf(tan2Theta))
        return 0;
    float cos4Theta = Sqr(Cos2Theta(wm));
    if (cos4Theta < 1e-16f)
        return 0;
    float e = tan2Theta * (Sqr(CosPhi(wm) / d.alpha_x) + Sqr(SinPhi(wm) / d.alpha_y));
    return 1 / (Pi * d.alpha_x * d.alpha_y * cos4Theta * Sqr(1 + e));
}

float TrowbridgeReitzDistribution_Lambda(TrowbridgeReitzDistribution d, vec3 w) 
{
    float tan2Theta = Tan2Theta(w);
    if (IsInf(tan2Theta))
        return 0;
    float alpha2 = Sqr(CosPhi(w) * d.alpha_x) + Sqr(SinPhi(w) * d.alpha_y);
    return (sqrt(1 + alpha2 * tan2Theta) - 1) / 2;
}

float TrowbridgeReitzDistribution_G1(TrowbridgeReitzDistribution d, vec3 w) 
{
    return 1 / (1 + TrowbridgeReitzDistribution_Lambda(d, w));
}

float TrowbridgeReitzDistribution_G(TrowbridgeReitzDistribution d, vec3 wo,  vec3 wi) 
{
    return 1 / (1 + TrowbridgeReitzDistribution_Lambda(d, wo) + TrowbridgeReitzDistribution_Lambda(d, wi)); 
}

float TrowbridgeReitzDistribution_D(TrowbridgeReitzDistribution d, vec3 w,  vec3 wm)
{
    return TrowbridgeReitzDistribution_G1(d, w) / AbsCosTheta(w) * TrowbridgeReitzDistribution_D(d, wm) * AbsDot(w, wm);
}

float TrowbridgeReitzDistribution_PDF(TrowbridgeReitzDistribution d, vec3 w,  vec3 wm) 
{
    return TrowbridgeReitzDistribution_D(d, w, wm); 
}

vec3 TrowbridgeReitzDistribution_Sample_wm(TrowbridgeReitzDistribution d, vec3 w, vec2 u) 
{
    // Transform _w_ to hemispherical configuration
    vec3 wh = Normalize(vec3(d.alpha_x * w.x, d.alpha_y * w.y, w.z));
    if (wh.z < 0)
        wh = -wh;

    // Find orthonormal basis for visible normal sampling
    vec3 T1 = (wh.z < 0.99999f) ? Normalize(Cross(vec3(0, 0, 1), wh)) : vec3(1, 0, 0);
    vec3 T2 = Cross(wh, T1);

    // Generate uniformly distributed points on the unit disk
    vec2 p = SampleUniformDiskPolar(u);

    // Warp hemispherical projection for visible normal sampling
    float h = sqrt(1 - Sqr(p.x));
    p.y = Lerp((1 + wh.z) / 2, h, p.y);

    // Reproject to hemisphere and transform normal to ellipsoid configuration
    float pz = sqrt(max(0, 1 - LengthSquared(p)));
    vec3 nh = p.x * T1 + p.y * T2 + pz * wh;

    return Normalize(vec3(d.alpha_x * nh.x, d.alpha_y * nh.y, max(1e-6f, nh.z)));
}

void TrowbridgeReitzDistribution_Regularize(TrowbridgeReitzDistribution d) 
{
    if (d.alpha_x < 0.3f)
        d.alpha_x = Clamp(2 * d.alpha_x, 0.1f, 0.3f);
    if (d.alpha_y < 0.3f)
        d.alpha_y = Clamp(2 * d.alpha_y, 0.1f, 0.3f);
}

#endif