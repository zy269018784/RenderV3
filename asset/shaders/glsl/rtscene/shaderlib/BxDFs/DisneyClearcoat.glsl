#ifndef DISNEY_CLEARCOAT_H
#define DISNEY_CLEARCOAT_H

#include "DisneyCommon.glsl"
  

vec3 SampleDisneyClearcoat(Material m, vec3 wo, inout vec3 wi, inout float pdf, inout float cos_theta, vec2 u) 
{
    float a2 = 0.0625;
	float cos_t = sqrt(max(0, (1.0 - pow(a2, 1.0f - u.x)) / (1.0 - a2)));
	float sin_t = sqrt(max(0, 1.0 - cos_theta * cos_theta));
	float phi = TWO_PI * u.y;
	vec3 h = vec3(sin_t * cos(phi), sin_t * sin(phi), cos_t);

	if ( dot(h, wo) < 0.0 ) {
		h = -h;
	}

	wi = reflect(-wo, h);
	if ( dot(wi, wo) < 0.0 ) {
		return vec3(0);
	}

    float clearcoat = GetDisneyClearcoat(m); 
    float clearcoatGloss = GetDisneyClearcoatGloss(m); 
	float D = GGX_D(mix(0.1, 0.001, clearcoatGloss), h.z); 
	float F = fresnel_schlick(0.04, 1.0, dot(wi, h));
	float G = GGX_G1(a2, wo.z) * GGX_G1(a2, wi.z);
 
	float f = 0.25 * clearcoat * D * F * G;
	pdf = D / (4.0 * dot(wo, h));
	cos_theta = wi.z; 
	return vec3(f);
}
 
vec3 EvalDisneyClearcoat(Material m, vec3 wo, vec3 wi, out float pdf, out float pdf_rev_w) 
{
	pdf = 0;
	pdf_rev_w = 0;
	const float a2 = 0.0625;
	vec3 h = normalize(wo + wi);

    float clearcoat = GetDisneyClearcoat(m); 
    float clearcoatGloss = GetDisneyClearcoatGloss(m); 

	float D = GGX_D(mix(0.1, 0.001, clearcoatGloss), h.z); 
	float F = fresnel_schlick(0.04, 1.0, dot(wi, h));
	float G = GGX_G1(a2, wo.z) * GGX_G1(a2, wi.z);

	float f = 0.25 * clearcoat * D * F * G;
	pdf = D / (4.0 * dot(wo, h));
	pdf_rev_w = pdf;  
	return vec3(f);
}

float EvalDisneyClearcoat_PDF(Material m, vec3 wo, vec3 wi) 
{
    float clearcoatGloss = GetDisneyClearcoatGloss(m); 
	vec3 h = normalize(wo + wi);
	float D = GGX_D(mix(0.1, 0.001, clearcoatGloss), h.z);
	return D / (4.0 * dot(wo, h));
}



#endif
