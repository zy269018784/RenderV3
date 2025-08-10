#ifndef DISNEY_BRDF_H
#define DISNEY_BRDF_H

#include "DisneyCommon.glsl"

bool bsdf_has_property(uint props, uint flag) { 
	return (props & flag) != 0;
}

vec3 CalcTint(vec3 albedo) 
{
	float lum = Luminance(albedo);
	return lum > 0 ? albedo / lum : vec3(1);
}

vec2 CalcAnisotropy(float roughness, float anisotropic) {
	float aspect = sqrt(1.0 - 0.9 * anisotropic);
	float a2 = roughness * roughness;
	return vec2(max(0.001, a2 / aspect), max(0.001, a2 * aspect));
}

bool IsBSDFDelta(float alpha) { return alpha == 0.0; }
// This prevents numerical problems with the GGX distribution
// In PBRTv4 it's 1e-4, in Falcor it's 6.4e-3
bool IsBSDFEffectiveDelta(float alpha) { return alpha <= 0.0064; }
bool IsBSDFEffectiveDelta(vec2 alpha) { return min(alpha.x, alpha.y) <= 0.0064; }

float GGX_D_Aniso(vec2 alpha, vec3 h) {
	float cos_sqr = h.z * h.z;
	float sin_sqr = max(1.0f - cos_sqr, 0.0f);
	float tan_sqr = sin_sqr / cos_sqr;
	if (isinf(tan_sqr)) {
		return 0.0f;
	}
	float cos_4 = cos_sqr * cos_sqr;
	if (cos_4 < 1e-16) {
		return 0.0;
	}
	vec2 phi_sqr = sin_sqr == 0.0 ? vec2(1.0, 0.0) : clamp(vec2(h.x * h.x, h.y * h.y), 0.0, 1.0) / sin_sqr;
	vec2 alpha_sqr = phi_sqr / (alpha * alpha);
	float e = tan_sqr * (alpha_sqr.x + alpha_sqr.y);
	return 1.0 / (Pi * alpha.x * alpha.y * cos_4 * (1.0 + e) * (1.0 + e));
}

float GGX_SmithG(float a2, float cosTheta){
	float c2 = cosTheta * cosTheta;
	return 1 / (cosTheta + sqrt(a2 + c2 - a2 * c2));
}

float GGX_Gr(float a,vec3 wo, vec3 wi){
	float a2 = a * a;
	return GGX_SmithG(a2,AbsCosTheta(wo));
}

float SmithLambda(float alpha_sqr, float cos_theta) 
{
	if (cos_theta == 0) {
		return 0;
	}
	float cos_sqr = cos_theta * cos_theta;
	float tan_sqr = max(1.0 - cos_sqr, 0) / cos_sqr;
	return 0.5 * (sqrt(1.0 + alpha_sqr * tan_sqr) - 1);
}

float SmithLambdaAniso(vec3 w, vec2 alpha) 
{
	float cos_sqr = w.z * w.z;
	float sin_sqr = max(1.0 - cos_sqr, 0);
	float tan_sqr = sin_sqr / cos_sqr;
	if (isinf(tan_sqr)) {
		return 0.0;
	}
	vec2 cos_phi_sqr = sin_sqr == 0.0 ? vec2(1.0, 0.0) : clamp(vec2(w.x * w.x, w.y * w.y), 0.0, 1.0) / sin_sqr;
	float alpha_sqr = dot(cos_phi_sqr, alpha * alpha);
	return 0.5 * (sqrt(1.0 + alpha_sqr * tan_sqr) - 1);
}

float GGX_G1_Aniso(vec3 w, vec2 alpha) { 
    return 1.0 / (1.0 + SmithLambdaAniso(w, alpha)); 
}

//The Distribution of Visible Normals，VNDF
vec3 sample_ggx_vndf_common(vec2 alpha, vec3 wo, vec2 xi) 
{
	vec3 wo_h = normalize(vec3(alpha.x * wo.x, alpha.y * wo.y, wo.z));
	float lensq = wo_h.x * wo_h.x + wo_h.y * wo_h.y;
	vec3 T1 = lensq > 0.0 ? vec3(-wo_h.y, wo_h.x, 0.0) * inversesqrt(lensq) : vec3(1.0, 0.0, 0.0);
	vec3 T2 = cross(wo_h, T1);
	float r = sqrt(xi.x);
	float phi = (2.0 * Pi) * xi.y;

	float t1 = r * cos(phi);
	float t2 = r * sin(phi);
	float s = 0.5 * (1.0 + wo_h.z);
	t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;
	vec3 n_h = t1 * T1 + t2 * T2 + sqrt(max(0.0, 1.0 - t1 * t1 - t2 * t2)) * wo_h;
	return normalize(vec3(alpha.x * n_h.x, alpha.y * n_h.y, max(0.0, n_h.z)));
}

vec3 sample_ggx_vndf_isotropic(vec2 alpha, vec3 wo, vec2 xi, out float pdf, out float D) {
	vec3 h = sample_ggx_vndf_common(alpha, wo, xi);
	pdf = eval_vndf_pdf_isotropic(alpha.x, wo, h, D);
	return h;
}

// pdf = G1(wo) * D(h) * max(0,dot(wo,h)) / wo.z
float Eval_VNDF_PDF(float a, vec3 wo, vec3 h, out float D)
{
	D = 0.0;
	if (wo.z <= 0) {
		return 0.0;
	} 
	float a2 = a * a;
	float G1 = GGX_G1(a2, wo.z);
	D = GGX_D(a2, h.z);
	return G1 * D * max(0.0, dot(wo, h)) / abs(wo.z);
} 

float Eval_VNDF_PDF_Aniso(vec2 alpha, vec3 wo, vec3 h, out float D) {
	D = 0.0;
	if (wo.z <= 0) {
		return 0.0;
	}
	float G1 = GGX_G1_Aniso(wo, alpha);
	D = GGX_D_Aniso(alpha, h);
	
	return G1 * D * max(0.0, dot(wo, h)) / abs(wo.z);
}

float Eval_VNDF_PDF_Aniso(vec2 alpha, vec3 wo, vec3 h)  
{
	float unused_D;
	return Eval_VNDF_PDF_Aniso(alpha, wo, h, unused_D);
}


//Sampling Visible GGX Normals with Spherical Caps
vec3 GGX_VNDF2(vec2 a, vec3 wo, vec2 u, out float pdf, out float D) {
	vec3 wo_h = normalize(vec3(a.x * wo.x, a.y * wo.y, wo.z)); 
  	float phi = TwoPi * u.x;
	float z = ((1.0 - u.y) * (1.0f + wo_h.z)) - wo_h.z;
	float sin_theta = sqrt(clamp(1.0f - z * z, 0.0, 1.0));
	float x = sin_theta * cos(phi);
	float y = sin_theta * sin(phi); 
    vec3 n_h = vec3(x, y, z) + wo_h; 
	vec3 h = normalize(vec3(a.x * n_h.x, a.y * n_h.y, max(0.0, n_h.z)));
	pdf = Eval_VNDF_PDF(a.x, wo, h, D);
	return h;
}

vec3 GGX_VNDF(vec2 a, vec3 wo, vec2 u, out float pdf, out float D) {
	vec3 wo_h = normalize(vec3(a.x * wo.x, a.y * wo.y, wo.z));
	float lensq = wo_h.x * wo_h.x + wo_h.y * wo_h.y; 
	vec3 T1 = lensq > 0.0 ? vec3(-wo_h.y, wo_h.x, 0.0) * inversesqrt(lensq) : vec3(1.0, 0.0, 0.0);
	vec3 T2 = cross(wo_h, T1); 
	float r = sqrt(u.x);
	float phi = TwoPi * u.y; 
	float t1 = r * cos(phi);
	float t2 = r * sin(phi); 
	float s = 0.5 * (1.0 + wo_h.z);
	t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;
	vec3 n_h = t1 * T1 + t2 * T2 + sqrt(max(0.0, 1.0 - t1 * t1 - t2 * t2)) * wo_h;  
	vec3 h = normalize(vec3(a.x * n_h.x, a.y * n_h.y, max(0.0, n_h.z))); 
	pdf = Eval_VNDF_PDF(a.x, wo, h, D);
	return h;
}

vec3 GGX_VNDF(vec2 a, vec3 wo, vec2 u) {
	vec3 wo_h = normalize(vec3(a.x * wo.x, a.y * wo.y, wo.z));
	float lensq = wo_h.x * wo_h.x + wo_h.y * wo_h.y; 
	vec3 T1 = lensq > 0.0 ? vec3(-wo_h.y, wo_h.x, 0.0) * inversesqrt(lensq) : vec3(1.0, 0.0, 0.0);
	vec3 T2 = cross(wo_h, T1); 
	float r = sqrt(u.x);
	float phi = TwoPi * u.y; 
	float t1 = r * cos(phi);
	float t2 = r * sin(phi); 
	float s = 0.5 * (1.0 + wo_h.z);
	t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;
	vec3 n_h = t1 * T1 + t2 * T2 + sqrt(max(0.0, 1.0 - t1 * t1 - t2 * t2)) * wo_h;  
	vec3 h = normalize(vec3(a.x * n_h.x, a.y * n_h.y, max(0.0, n_h.z))); 
	return h;
}

vec3 GGX_VNDF_Aniso(vec2 alpha, vec3 wo, vec2 xi, out float pdf, out float D)
{
	vec3 h = GGX_VNDF(alpha, wo, xi);
	pdf = Eval_VNDF_PDF_Aniso(alpha, wo, h, D);
	return h;
}



float GGX_G_correlated(float alpha, vec3 wo, vec3 wi) {
	float a2 = alpha * alpha;
	return 1.0 / (1.0 + SmithLambda(a2, wo.z) + SmithLambda(a2, wi.z));
}

float GGX_G_Aniso(vec3 wo, vec3 wi, vec2 alpha) {
	return 1.0 / (1.0 + SmithLambdaAniso(wo, alpha) + SmithLambdaAniso(wi, alpha));
}

// Always used in BRDF context
vec3 DisneyFresnel(vec3 albedo, float metallic, float specularTint, vec3 wo, vec3 h, vec3 wi, float eta) 
{ 
	float wo_dot_h = dot(wo, h); // Always equivalent to dot(wi, h)
	vec3 albedoTint = CalcTint(albedo); 
	vec3 R0 = SchlickR0(eta) * mix(vec3(1.0), albedoTint, specularTint); 
	R0 = mix(R0, albedo, metallic);

	// Eta already accounts for which face it corresponds to. Therefore forward_facing is set to true
	float fr_dielectric = fresnel_dielectric(wo_dot_h, eta);
	vec3 fr_metallic = fresnel_schlick(R0, vec3(1), wo_dot_h); 
	return mix(vec3(fr_dielectric), fr_metallic, metallic);
}

vec3 SampleDisneyBRDF(Material m, vec3 wo, inout vec3 wi, inout float pdf_w, inout float cos_theta, const vec2 u, float eta) 
{ 
	int bsdf_props = GetDisneyBSDFProperties(m);
	bool has_reflection = bsdf_has_property(bsdf_props, BSDF_FLAG_REFLECTION);
	if (!has_reflection) { //没有反射属性直接过滤
		return vec3(0);
	}
	vec3 albedo = GetDisneyAlbedo(m); 
	float metallic = GetDisneyMetallic(m);
	float specularTint = GetDisneySpecularTint(m);
	float roughness = GetDisneyRoughness(m);
	float anisotropy = GetDisneyAnisotropy(m);
	
	float D;
	vec2 alpha = CalcAnisotropy(roughness, anisotropy);
	if (IsBSDFEffectiveDelta(alpha)) { //镜面或理想折射，这种情况下pdf是确定的
		wi = vec3(-wo.x, -wo.y, wo.z);
		pdf_w = 1.0;
		cos_theta = wi.z;
		vec3 F = DisneyFresnel(albedo, metallic, specularTint, wo, vec3(0, 0, 1), wi, eta);
		return F / abs(cos_theta);
	}
	
	//计算出HalfVertor
	vec3 h = GGX_VNDF_Aniso(alpha, wo, u, pdf_w, D); 
	wi = reflect(-wo, h); 
	if (wo.z * wi.z < 0) {// Make sure the reflection lies in the same hemisphere
		return vec3(0);
	}

	vec3 F = DisneyFresnel(albedo, metallic, specularTint, wo, h, wi, eta);
	pdf_w /= (4.0 * dot(wo, h));
	cos_theta = wi.z;
	return 0.25 * D * F * GGX_G_Aniso( wo, wi, alpha) / (wi.z * wo.z);
}

vec3 EvalDisneyBRDF(Material m, vec3 wo, vec3 wi, out float pdf_w, out float pdf_rev_w, uint mode, bool eval_reverse_pdf, float eta) 
{
	pdf_w = 0;
	pdf_rev_w = 0;
 
 	vec3 albedo = GetDisneyAlbedo(m); 
	float metallic = GetDisneyMetallic(m);
	float specularTint = GetDisneySpecularTint(m);
	float roughness = GetDisneyRoughness(m);
	float anisotropy = GetDisneyAnisotropy(m);
	 
	vec2 alpha = CalcAnisotropy(roughness, anisotropy); 
	if (IsBSDFEffectiveDelta(alpha)) {
		return vec3(0);
	}
	if (wo.z * wi.z < 0) {
		return vec3(0);
	}
	if (wo.z == 0 || wi.z == 0) {
		return vec3(0);
	}
	vec3 h = normalize(wo + wi);
	float jacobian = 1.0 / (4.0 * dot(wo, h));
	float D; 
	pdf_w = Eval_VNDF_PDF_Aniso(alpha, wo, h, D) * jacobian;
 
	if (eval_reverse_pdf) { 
		pdf_rev_w = Eval_VNDF_PDF_Aniso(alpha, wi, h) * jacobian; 
	}
	vec3 F = DisneyFresnel(albedo, metallic, specularTint, wo, h, wi, eta);
	return 0.25 * D * F * GGX_G_Aniso(wo, wi, alpha) / (wi.z * wo.z); 
}

float EvalDisneyBRDF_PDF(Material m, vec3 wo, vec3 wi) 
{ 
	float roughness = GetDisneyRoughness(m);
	float anisotropy = GetDisneyAnisotropy(m); 

	vec2 alpha = CalcAnisotropy(roughness, anisotropy);  
	if (IsBSDFEffectiveDelta(alpha)) {
		return 0.0;
	} 
	if (wo.z * wi.z < 0) {
		return 0.0;
	}
	if (wo.z == 0 || wi.z == 0) {
		return 0.0;
	} 
	vec3 h = normalize(wo + wi); 
	h *= float(sign(h.z)); // Make sure h is oriented towards the normal
	return Eval_VNDF_PDF_Aniso(alpha, wo, h) / (4.0 * dot(wo, h)); 
} 

#endif 
