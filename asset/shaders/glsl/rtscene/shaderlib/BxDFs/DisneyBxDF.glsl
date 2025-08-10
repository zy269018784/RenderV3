#ifndef DISNEY_BxDF_H
#define DISNEY_BxDF_H

#include "DisneyDiffuse.glsl"
#include "DisneyBrdf.glsl"
#include "DisneyClearcoat.glsl"
#include "DisneyDielectric.glsl"

void CalcSamplingProbabilities( float metallic, float transmission, float clearcoat, float F_dielectric, 
                                bool forward_facing, 
                                out float p_reflection, out float p_diffuse, out float p_clearcoat, out float p_transmission) 
{
	float brdf_weight = (1.0 - transmission) * (1.0 - metallic);  //反射权重
	float bsdf_weight = (1.0 - metallic) * transmission;          //散射权重
 
	p_diffuse       = forward_facing ? brdf_weight : 0.0;
	p_reflection    = forward_facing ? (1.0 - bsdf_weight * (1.0 - F_dielectric)) : F_dielectric;
	p_transmission  = forward_facing ? (bsdf_weight * (1.0 - F_dielectric)) : (1.0 - F_dielectric);
	p_clearcoat     = forward_facing ? 0.25 * clearcoat : 0.0;

	float norm = 1.0 / (p_diffuse + p_reflection + p_transmission + p_clearcoat); 
	p_diffuse *= norm;
    p_reflection *= norm;
	p_clearcoat *= norm;
	p_transmission *= norm;
}   


BSDFSample DisneyBxDF_Sample_F(vec3 wo, float uc, vec2 u, Material m)
{
    BSDFSample bs;
    bs.wi = vec3(0);
    bs.pdf = 0.0;
    bs.f = vec3(0.0); 

    float cos_theta = 0.0;
    if(wo.z <= 0.0) {
        return bs;
    } 
 
    bool forward_facing = true;
    float ior = max(GetDisneyIOR(m), 1e-5); 
    float eta = forward_facing ? ior : 1.0 / ior;

    float F = FrDielectric(wo.z, ior); 

    vec3 albedo = GetAlbedo(m);
    float metallic = GetDisneyMetallic(m);
    float roughness = GetDisneyRoughness(m);
    float transmission = GetDisneyTransmission(m);
    float specular = GetDisneySpecular(m);
    float specularTint = GetDisneySpecularTint(m);
    float sheen = GetDisneySheen(m);
    float sheenTint = GetDisneySheenTint(m);
    float clearcoat = GetDisneyClearcoat(m);
    float clearcoatGloss = GetDisneyClearcoatGloss(m);

    float p_reflection, p_diffuse, p_clearcoat, p_transmission;
    CalcSamplingProbabilities(metallic, transmission, clearcoat, F, forward_facing, p_reflection, p_diffuse, p_clearcoat, p_transmission); //计算采样概率
 
    float pdf = 0.0; 
    if(uc < p_reflection) {                                                                           //反射
        bs.f = SampleDisneyBRDF(m, wo, bs.wi, bs.pdf, cos_theta, u, eta);
        pdf = p_reflection;
    } 
    else if(uc > p_reflection && uc <= (p_reflection + p_clearcoat)) //清漆
    {                                    
        bs.f = SampleDisneyClearcoat(m, wo, bs.wi, bs.pdf, cos_theta, u);
        pdf = p_clearcoat;
    } 
    else if(uc > (p_reflection + p_clearcoat) && uc <= (p_reflection + p_clearcoat + p_diffuse)) //漫反射
    {           
        bs.f = SampleDisneyDiffuse(m, wo, bs.wi, bs.pdf, cos_theta, u);
        pdf = p_diffuse;
    } 
    else if(p_transmission >= 0.0 && uc <= (p_reflection + p_clearcoat + p_diffuse + p_transmission)) //电介质 折射
    {    
        bs.f = SampleDisneyDielectric(m, wo, bs.wi, bs.pdf, cos_theta, u);
        pdf = p_transmission;
    }

    bs.pdf *= pdf;  
    return bs; 
}

  
vec3 DisneyBxDF_F(vec3 wo, vec3 wi, Material m)
{
	float pdf_w = 0.0;  //out 

    bool forward_facing = true;
    float ior = GetDisneyIOR(m);
    float eta = forward_facing ? ior : 1.0 / ior;

    float roughness = GetDisneyRoughness(m);
	float alpha = roughness * roughness;
    float F = FrDielectric(wo.z, ior);  

    vec3 albedo = GetAlbedo(m);
    float metallic = GetDisneyMetallic(m); 
    float transmission = GetDisneyTransmission(m);
    float specular = GetDisneySpecular(m);
    float specularTint = GetDisneySpecularTint(m);
    float sheen = GetDisneySheen(m);
    float sheenTint = GetDisneySheenTint(m);
    float clearcoat = GetDisneyClearcoat(m);
    float clearcoatGloss = GetDisneyClearcoatGloss(m);

    float p_reflection, p_diffuse, p_clearcoat, p_transmission; 
    CalcSamplingProbabilities(metallic, transmission, clearcoat, F, forward_facing, p_reflection, p_diffuse, p_clearcoat, p_transmission); //计算采样概率

	vec3 f = vec3(0);
	float pdf = 0;
	float pdf_rev = 0;

	float brdf_weight = (1.0 - transmission) * (1.0 - metallic);
	float bsdf_weight = (1.0 - metallic) * transmission;

    int mode = 0; 
    bool eval_reverse_pdf = false;
	if (p_reflection > 0) { 
		f += EvalDisneyBRDF(m, wo, wi, pdf, pdf_rev, mode, eval_reverse_pdf, eta);
		pdf *= p_reflection;
		pdf_rev *= p_reflection;

		pdf_w += pdf; 
	}

	bool upper_hemisphere = min(wi.z, wo.z) > 0;
	if (upper_hemisphere) {
		if (p_diffuse > 0) {
			pdf_w += p_diffuse * EvalDisneyDiffuse_PDF(wo, wi); 

			f += brdf_weight * CalcDisneyDiffuseFactor(m, wo, wi);
		}
		if (p_clearcoat > 0) {
			f += EvalDisneyClearcoat(m, wo, wi, pdf, pdf_rev); 
			pdf *= p_clearcoat;
			pdf_rev *= p_clearcoat;

			pdf_w += pdf; 
		}
	}

	if (p_transmission > 0) {
		f += bsdf_weight * EvalDisneyDielectric(m, wo, wi, pdf, pdf_rev, forward_facing, mode, eval_reverse_pdf);
		pdf *= p_transmission;
		pdf_rev *= p_transmission;

		pdf_w += pdf; 
	}
	return f;
} 

float DisneyBxDF_PDF(vec3 wo, vec3 wi, Material m)
{
    float pdf = 0.0;

    bool forward_facing = true;
    float ior = GetDisneyIOR(m);
    float roughness = GetDisneyRoughness(m);
	float alpha = roughness * roughness;
    float F = FrDielectric(wo.z, ior);  

    vec3 albedo = GetAlbedo(m);
    float metallic = GetDisneyMetallic(m); 
    float transmission = GetDisneyTransmission(m);
    float specular = GetDisneySpecular(m);
    float specularTint = GetDisneySpecularTint(m);
    float sheen = GetDisneySheen(m);
    float sheenTint = GetDisneySheenTint(m);
    float clearcoat = GetDisneyClearcoat(m);
    float clearcoatGloss = GetDisneyClearcoatGloss(m);

    float p_reflection, p_diffuse, p_clearcoat, p_transmission; 
    CalcSamplingProbabilities(metallic, transmission, clearcoat, F, forward_facing, p_reflection, p_diffuse, p_clearcoat, p_transmission); //计算采样概率
 
	if (p_reflection > 0) {
		pdf += p_reflection * EvalDisneyBRDF_PDF(m, wo, wi);
	}

	bool upper = min(wi.z, wo.z) > 0;
	if (upper) {
		if (p_diffuse > 0) {
			pdf += p_diffuse * EvalDisneyDiffuse_PDF(wo, wi);
		}
		if (p_clearcoat > 0) {
			pdf += p_clearcoat * EvalDisneyClearcoat_PDF(m, wo, wi);
		}
	}
	if (p_transmission > 0) {
		pdf += p_transmission * EvalDisneyDielectric_PDF(m, wo, wi, forward_facing);
	}
	return pdf; 
} 
 

 
#endif