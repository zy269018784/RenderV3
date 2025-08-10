#ifndef ColoredAluminumPlate1_BxDF_H
#define ColoredAluminumPlate1_BxDF_H 


BSDFSample ColoredAluminumPlate1BxDF_Sample_F(vec3 wo, float uc, vec2 u2, BSDFSampleInput SampleInput)
{
    vec3 wi = SampleCosineHemisphere(u2); 
    if (wo.z < 0)
        wi.z *= -1; 

    vec3 Albedo = GetMaterialAlbedo(SampleInput.m, SampleInput.uv);

    float pdf = CosineHemispherePDF(AbsCosTheta(wi)); 
    vec3 f = vec3(InvPi);

    //if (uc < 0.95f)
    //Albedo = vec3(0, 1, 0);
    f *= Albedo;

    return BSDFSample(wi, pdf, f); 
}

vec3 ColoredAluminumPlate1BxDF_F(vec3 wo, vec3 wi, Material m)
{
    if (!SameHemisphere(wo, wi))
        return vec3(0);
    return vec3(CosineHemispherePDF(AbsCosTheta(wi)));
}

float ColoredAluminumPlate1BxDF_PDF(vec3 wo, vec3 wi, Material m)
{
    if (!SameHemisphere(wo, wi))
        return 0;
    return CosineHemispherePDF(AbsCosTheta(wi));
}


#endif