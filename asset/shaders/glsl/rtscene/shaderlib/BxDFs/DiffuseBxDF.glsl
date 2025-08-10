#ifndef DIFFUSE_BxDF_H
#define DIFFUSE_BxDF_H
 

BSDFSample DiffuseBxDF_Sample_F(vec3 wo, float u, vec2 u2, BSDFSampleInput SampleInput)
{
    vec3 Albedo = GetMaterialAlbedo(SampleInput.m, SampleInput.uv);
    vec3 wi = SampleCosineHemisphere(u2); 
    if (wo.z < 0)
        wi.z *= -1; 
    float pdf = CosineHemispherePDF(AbsCosTheta(wi)); 
    vec3 f = vec3(InvPi) * Albedo;
    return BSDFSample(wi, pdf, f); 
}

vec3 DiffuseBxDF_F(vec3 wo, vec3 wi, Material m)
{
    if (!SameHemisphere(wo, wi))
        return vec3(0);
    return vec3(CosineHemispherePDF(AbsCosTheta(wi)));
}

float DiffuseBxDF_PDF(vec3 wo, vec3 wi, Material m)
{
    if (!SameHemisphere(wo, wi))
        return 0;
    return CosineHemispherePDF(AbsCosTheta(wi));
}

#endif