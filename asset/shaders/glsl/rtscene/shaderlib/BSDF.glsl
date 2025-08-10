#ifndef BSDF_H
#define BSDF_H

#include "Frame.glsl"
#include "BxDF.glsl"

struct BSDF
{
	Frame ShadingFrame;
};

BSDF ConstructBSDF(vec3 dpdus, vec3 ns)
{
	BSDF bsdf;
	//bsdf.ShadingFrame = FromXZ(normalize(dpdus), ns);
	bsdf.ShadingFrame = FromZ(ns);
	return bsdf;
}

BSDFSample SampleBSDF(BSDF bsdf, vec3 wo, float u, vec2 u2, BSDFSampleInput SampleInput)
{
	BSDFSample bs;
	wo = ToLocal(bsdf.ShadingFrame, wo);
	if (wo.z == 0)
        return bs;
    bs = BXDFSample(wo, u, u2, SampleInput);
	bs.wi = FromLocal(bsdf.ShadingFrame, bs.wi);
	return bs;
}

vec3 Sample_F(vec3 wo, vec3 wi, Material m)
{
	if (wo.z == 0)
        return vec3(0);
	return BXDFSample_F(wo, wi, m);
}

float Sample_PDF(vec3 wo, vec3 wi, Material m)
{
	if (wo.z == 0)
        return 0;
	return BXDFSample_PDF(wo, wi, m);
}

#endif



 





