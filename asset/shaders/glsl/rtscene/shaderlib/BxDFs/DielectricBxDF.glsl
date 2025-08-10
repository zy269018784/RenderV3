#ifndef DIELECTRIC_BxDF_H
#define DIELECTRIC_BxDF_H

/*
    Dielectric
*/
BSDFSample DielectricBxDF_Sample_F(vec3 wo, float uc, vec2 u, BSDFSampleInput SampleInput)
{
    BSDFSample sample1;
    TrowbridgeReitzDistribution d;
    vec3 Albedo = GetMaterialAlbedo(SampleInput.m, SampleInput.uv);
    d.alpha_x = DielectricGetURoughness(SampleInput.m);
    d.alpha_y = DielectricGetVRoughness(SampleInput.m); 
    float eta = 1.5;
    float R = FrDielectric(CosTheta(wo), eta);
    float T = 1.f - R;
    if (wo.z == 0)
        return sample1; 
    float reflectivity = DielectricGetReflectivity(SampleInput.m);
    if(reflectivity > uc) {
        vec3 wi = vec3(-wo.x, -wo.y, wo.z);
        float pdf = 0.5f;
        vec3 f = vec3(reflectivity);
        f *= Albedo;
        return BSDFSample(wi, pdf, f);
    }
    else {
        vec3 wm = TrowbridgeReitzDistribution_Sample_wm(d, wo, u);
        float R = FrDielectric(Dot(wo, wm), eta);
        float T = 1 - R;
        vec3 wi = Reflect(wo, wm);
        if (!SameHemisphere(wo, wi))
            return DiffuseBxDF_Sample_F(wo, uc, u, SampleInput); //todo: fixme
        float pdf = TrowbridgeReitzDistribution_PDF(d, wo, wm) / (4 * AbsDot(wo, wm)) * R / (R + T);
        vec3 f = vec3(TrowbridgeReitzDistribution_D(d, wm) * TrowbridgeReitzDistribution_G(d, wo, wi) * R / (4 * CosTheta(wi) * CosTheta(wo)));
        f *= Albedo;
        return BSDFSample(wi, pdf, f);
    } 
}

vec3 DielectricBxDF_F(vec3 wo, vec3 wi, Material m)
{
    float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
    bool b_reflect = cosTheta_i * cosTheta_o > 0;
    float etap = 1;
    float eta = 1.5;
    if (!b_reflect)
        etap = cosTheta_o > 0 ? eta : (1 / eta);
    vec3 wm = wi * etap + wo;
    if (cosTheta_i == 0 || cosTheta_o == 0 || LengthSquared(wm) == 0)
        return vec3(0);
    wm = faceforward(normalize(wm), vec3(0, 0, 1));
 
    if (dot(wm, wi) * cosTheta_i < 0 || dot(wm, wo) * cosTheta_o < 0)
        return vec3(0);

    float F = FrDielectric(dot(wo, wm), eta);
    if (b_reflect) { 
        TrowbridgeReitzDistribution d;
        d.alpha_x = DielectricGetURoughness(m);
        d.alpha_y = DielectricGetVRoughness(m); 
        float fDiffuse = CosineHemispherePDF(AbsCosTheta(wi));
        return vec3(TrowbridgeReitzDistribution_D(d, wm) * TrowbridgeReitzDistribution_G(d, wo, wi) * F / abs(4 * cosTheta_i * cosTheta_o) + fDiffuse); 
    } else {
        return vec3(0);
    }
}

float DielectricBxDF_PDF(vec3 wo, vec3 wi, Material m)
{
    float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
    bool b_reflect = cosTheta_i * cosTheta_o > 0;
    float etap = 1;
    float eta = 1.5;
    if (!b_reflect)
        etap = cosTheta_o > 0 ? eta : (1 / eta);
    vec3 wm = wi * etap + wo;
    if (cosTheta_i == 0 || cosTheta_o == 0 || LengthSquared(wm) == 0)
        return 0;
    wm = faceforward(normalize(wm), vec3(0, 0, 1));

    // Discard backfacing microfacets
    if (dot(wm, wi) * cosTheta_i < 0 || dot(wm, wo) * cosTheta_o < 0)
        return 0;
 
    float pr = FrDielectric(dot(wo, wm), eta);
    float pt = 1 - pr;
    float pdf;
    if (b_reflect) {
        TrowbridgeReitzDistribution d;
        d.alpha_x = DielectricGetURoughness(m);
        d.alpha_y = DielectricGetVRoughness(m); 
        pdf = TrowbridgeReitzDistribution_PDF(d, wo, wm) / (4 * AbsDot(wo, wm)) * pr / (pr + pt);
    } else {
        return 0;
    }
    return pdf;
} 

#endif