#ifndef CONDUCTOR_BxDF_H
#define CONDUCTOR_BxDF_H 

BSDFSample ConductorBxDF_Sample_F(vec3 wo, float uc, vec2 u, BSDFSampleInput SampleInput)
{
    BSDFSample sample1;
    sample1.wi = vec3(0);
    sample1.pdf = 0.0;
    sample1.f = vec3(0.0);

    vec3 Albedo = GetMaterialAlbedo(SampleInput.m, SampleInput.uv);
    float uRoughness = ConductorGetURoughness(SampleInput.m);
    float vRoughness = ConductorGetVRoughness(SampleInput.m);
    TrowbridgeReitzDistribution d = TrowbridgeReitzDistribution_Init(uRoughness,vRoughness); 
    if (TrowbridgeReitzDistribution_EffectivelySmooth(d)) {
        vec3 wi = vec3(-wo.x, -wo.y, wo.z);
        vec3 f = vec3(FrComplex(AbsCosTheta(wi), Complex(1.373, 7.618))); 
        return BSDFSample(wi, 1, f);
    }

    if (wo.z == 0)
        return sample1;
    
    vec3 wm = TrowbridgeReitzDistribution_Sample_wm(d, wo, u);
    vec3 wi = Reflect(wo, wm);
    if (!SameHemisphere(wo, wi))
        return sample1; 

    float pdf = TrowbridgeReitzDistribution_PDF(d, wo, wm) / (4 * AbsDot(wo, wm));

    float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
    if (cosTheta_i == 0 || cosTheta_o == 0)
        return sample1;
 
    Complex al_eta = Complex( 1.373, 7.618); 
    float F = FrComplex(AbsDot(wo, wm), al_eta);
    float D = TrowbridgeReitzDistribution_D(d, wm);
    float G = TrowbridgeReitzDistribution_G(d, wo, wi);  
    vec3 f = vec3(D * F * G / (4 * max(cosTheta_i, 0.0) * max(cosTheta_o, 0.0)));
    f *= Albedo;
    sample1.wi = wi;
    sample1.pdf = pdf;
    sample1.f = f;

    return sample1;
}

vec3 ConductorBxDF_F(vec3 wo, vec3 wi, Material m)
{
    if (!SameHemisphere(wo, wi))
        return vec3(0);  
    float uRoughness = ConductorGetURoughness(m);
    float vRoughness = ConductorGetVRoughness(m);
    TrowbridgeReitzDistribution d = TrowbridgeReitzDistribution_Init(uRoughness,vRoughness);  
    if (TrowbridgeReitzDistribution_EffectivelySmooth(d))
        return vec3(0);
    float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
    if (cosTheta_i == 0 || cosTheta_o == 0)
        return vec3(0);
    vec3 wm = wi + wo;
    if (LengthSquared(wm) == 0)
        return vec3(0);
    wm = normalize(wm);
    Complex al_eta = { 1.373, 7.618 };
    float F = FrComplex(AbsDot(wo, wm), al_eta); 
    float f = TrowbridgeReitzDistribution_D(d, wm) * F * TrowbridgeReitzDistribution_G(d, wo, wi) / (4 * cosTheta_i * cosTheta_o);
    return vec3(f + InvPi);
}

float ConductorBxDF_PDF(vec3 wo, vec3 wi, Material m)
{
    if (!SameHemisphere(wo, wi))
        return 0; 
    float uRoughness = ConductorGetURoughness(m);
    float vRoughness = ConductorGetVRoughness(m);
    TrowbridgeReitzDistribution d = TrowbridgeReitzDistribution_Init(uRoughness,vRoughness); 
    if (TrowbridgeReitzDistribution_EffectivelySmooth(d))
        return 0;  
    vec3 wm = wo + wi;
    if (LengthSquared(wm) == 0)
        return 0; 
    wm = faceforward(normalize(wm), vec3(0, 0, 1));
    return TrowbridgeReitzDistribution_PDF(d, wo, wm) / (4 * AbsDot(wo, wm));
}


#endif