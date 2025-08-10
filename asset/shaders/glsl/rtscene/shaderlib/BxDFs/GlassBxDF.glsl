#ifndef GLASS_BxDF_H
#define GLASS_BxDF_H
 
BSDFSample GlassBxDF_Sample_F(vec3 wo, float uc, vec2 u2, BSDFSampleInput SampleInput)
{
	vec3 wi;
	float pdf;
	vec3 f;
    float r = GlassGetReflectivity(SampleInput.m);
    float t = GlassGetTransmissivity(SampleInput.m);
    float ratio = r / (r + t);
    if (uc < ratio)  { // 反射
		wi = vec3(-wo.x, -wo.y, wo.z); 
		pdf = 0.5f;
        pdf = ratio;
		f = vec3(GlassGetReflectivity(SampleInput.m));
    }
    else  { // 透射
		wi = -wo; 
		pdf = 0.5f;
        pdf = 1 - ratio;
		f = vec3(GlassGetTransmissivity(SampleInput.m));
    }
	return BSDFSample(wi, pdf, f);
}


vec3 GlassBxDF_F(vec3 wo, vec3 wi, Material m)
{
    if (!SameHemisphere(wo, wi))
        return vec3(0);
    return vec3(0.5f);
}

float GlassBxDF_PDF(vec3 wo, vec3 wi, Material m)
{
    if (!SameHemisphere(wo, wi))
        return 0;
    return 0.5f;
}















#endif
