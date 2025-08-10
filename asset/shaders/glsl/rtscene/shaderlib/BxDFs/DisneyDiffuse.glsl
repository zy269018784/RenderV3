#ifndef DISNEY_DIFFUSE_H
#define DISNEY_DIFFUSE_H

#include "DisneyCommon.glsl"

vec3 CalcDisneyDiffuseFactor(Material m, vec3 wo, vec3 wi)
{
	vec3 h = normalize(wi + wo);

	float roughness = GetDisneyRoughness(m);
	float flatness = GetDisneyFlatness(m);
	vec3 albedo = GetDisneyAlbedo(m);

	float f_wi, f_wo;
	float disney_f = disney_fresnel(wi, wo, roughness, f_wi, f_wo);
	float a = roughness * roughness;

	float ss = 0;

	// Retro-reflection
	float rr = 2.0 * a * wi.z * wi.z;
	float f_retro = rr * (f_wi + f_wo * f_wi * f_wo * (rr - 1.0)); 
	float f_diff = (1.0 - 0.5 * f_wi) * (1.0 - 0.5 * f_wo);
	if (flatness > 0.0) {
		float fss90 = 0.5 * rr;
		float h_dot_wi_sqr = dot(h, wi);
		h_dot_wi_sqr *= h_dot_wi_sqr; 
		float f_ss = mix(1.0, fss90, f_wi) * mix(1.0, fss90, f_wo);
		ss = 1.25 * (f_ss * (1.0 / (wi.z + wo.z) - 0.5) + 0.5);
	}
	float ss_approx_and_diff = mix(f_diff + f_retro, ss, flatness);
	return albedo * ss_approx_and_diff * INV_PI;
}


// In addition to diffuse, this includes retro reflection, fake subsurface and sheen
vec3 SampleDisneyDiffuse(Material m, vec3 wo, out vec3 wi, out float pdf_w, out float cos_theta, vec2 u) 
{
	wi = SampleCosineHemisphere(u);
	cos_theta = wi.z;
	pdf_w = cos_theta * INV_PI;
	if (min(wi.z, wo.z) <= 0.0) {
		return vec3(0);
	}
		
	vec3 h = normalize(wi + wo);
	float roughness = GetDisneyRoughness(m);
	float flatness = GetDisneyFlatness(m);
	vec3 albedo = GetDisneyAlbedo(m);

	float f_wi, f_wo;
	float disney_f = disney_fresnel(wi, wo, roughness, f_wi, f_wo);
	float a2 = roughness * roughness;

	float ss = 0;

	// Retro-reflection
	float rr = 2.0 * a2 * wi.z * wi.z;
	float f_retro = rr * (f_wi + f_wo * f_wi * f_wo * (rr - 1.0)); 
	float f_diff = (1.0 - 0.5 * f_wi) * (1.0 - 0.5 * f_wo);

	if (flatness > 0.0) {
		float fss90 = 0.5 * rr;
		float h_dot_wi_sqr = dot(h, wi);
		h_dot_wi_sqr *= h_dot_wi_sqr;

		float f_ss = mix(1.0, fss90, f_wi) * mix(1.0, fss90, f_wo);
		ss = 1.25 * (f_ss * (1.0 / (wi.z + wo.z) - 0.5) + 0.5);
	}

	float ss_approx_and_diff = mix(f_diff + f_retro, ss, flatness);
	return albedo * ss_approx_and_diff * INV_PI; 
}
 
vec3 EvalDisneyDiffuse(Material m, vec3 wo, vec3 wi, out float pdf_w, out float pdf_rev_w) 
{
	if (min(wi.z, wo.z) <= 0.0) {
		return vec3(0);
	}
	pdf_w = wi.z * INV_PI;
	pdf_rev_w = pdf_w;
	return GetDisneyAlbedo(m) * disney_fresnel(wi, wo, GetDisneyRoughness(m)) * INV_PI;
} 

float EvalDisneyDiffuse_PDF(vec3 wo, vec3 wi) {
    if (min(wi.z, wo.z) <= 0.0) {
		return 0.0;
	}
	return wi.z * INV_PI;
}
 


#endif