#ifndef DISNEY_COMMON_H
#define DISNEY_COMMON_H
#include "../Math.glsl"

#define ANISOTROPIC 1

float Luminance(vec3 rgb){
	return 0.299 * rgb.x + 0.587 * rgb.y + 0.114 * rgb.z;
} 

// 折射: eta (relative IOR) is nu_i / nu_o
bool Refract(vec3 n_s, vec3 wo, float eta, uint mode, out vec3 wi, out vec3 f, out float inv_eta) {
	wi = vec3(0);
	f = vec3(0);

	// Relative IOR (Inside / outside). If the normal direction has been changed, we take the inverse
	float cos_i = dot(n_s, wo);
	inv_eta = 1.0 / eta;
	const float sin2_t = inv_eta * inv_eta * (1. - cos_i * cos_i);
	if (sin2_t >= 1.) { 
		wi = reflect(-wo, n_s); 
	} else {
		const float cos_t = sqrt(1 - sin2_t);
		wi = -inv_eta * wo + (inv_eta * cos_i - cos_t) * n_s;
	}
	f = mode == 1 ? vec3(inv_eta * inv_eta) : vec3(1);
	return true;
}

bool Refract(vec3 n_s, vec3 wo, float eta, uint mode, out vec3 wi, out vec3 f) {
	wi = vec3(0);
	f = vec3(0);
 
	float cos_i = dot(n_s, wo);
	float inv_eta = 1.0 / eta;
	const float sin2_t = inv_eta * inv_eta * (1. - cos_i * cos_i);
	if (sin2_t >= 1.) { 
		wi = reflect(-wo, n_s); 
	} else {
		const float cos_t = sqrt(1 - sin2_t);
		wi = -inv_eta * wo + (inv_eta * cos_i - cos_t) * n_s;
	}
	f = mode == 1 ? vec3(inv_eta * inv_eta) : vec3(1);
	return true;
} 


float fresnel_dielectric(float cos_i, float eta) {
	cos_i = clamp(cos_i, -1.0, 1.0); 
	if (cos_i < 0) {
		eta = 1.0 / eta;
		cos_i = -cos_i;
	}

	float sin2_i = 1 - cos_i * cos_i;
	float sin2_t = sin2_i / (eta * eta);
	if (sin2_t >= 1) return 1.f;
	float cos_t = sqrt(1 - sin2_t);
	float r_parallel = (eta * cos_i - cos_t) / (eta * cos_i + cos_t);
	float r_perp = (cos_i - eta * cos_t) / (cos_i + eta * cos_t);
	return 0.5 * (r_parallel * r_parallel + r_perp * r_perp);
}

// https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
float fresnel_conductor(float cos_i, float eta, float k) {
	float cos_sqr = cos_i * cos_i;
	float sin_sqr = max(1.0f - cos_sqr, 0.0f);
	float sin_4 = sin_sqr * sin_sqr;

	float inner_term = eta * eta - k * k - sin_sqr;
	float a_sq_p_b_sq = sqrt(max(inner_term * inner_term + 4.0f * eta * eta * k * k, 0.0f));
	float a = sqrt(max((a_sq_p_b_sq + inner_term) * 0.5f, 0.0f));

	float rs = ((a_sq_p_b_sq + cos_sqr) - (2.0f * a * cos_i)) / ((a_sq_p_b_sq + cos_sqr) + (2.0f * a * cos_i));
	float rp = ((cos_sqr * a_sq_p_b_sq + sin_4) - (2.0f * a * cos_i * sin_sqr)) /
			   ((cos_sqr * a_sq_p_b_sq + sin_4) + (2.0f * a * cos_i * sin_sqr));

	return 0.5f * (rs + rs * rp);
}

float fresnel_schlick(float f0, float f90, float ns) 
{
	// Makes sure that (1.0 - n_s) >= 0
	return f0 + (f90 - f0) * pow(max(1.0 - ns, 0), 5.0f);
}

vec3 fresnel_schlick(vec3 f0, vec3 f90, float ns) 
{
	// Makes sure that (1.0 - n_s) >= 0
	return f0 + (f90 - f0) * pow(max(1.0 - ns, 0), 5.0f);
}

float schlick_w(float u) 
{
	float m = clamp(1 - u, 0, 1);
	float m2 = m * m;   
	return m2 * m2 * m;	// m * m * m * m * m;
}

bool bsdf_is_delta(float alpha) 
{ 
	return alpha == 0.0; 
}

// This prevents numerical problems with the GGX distribution In PBRTv4 it's 1e-4, in Falcor it's 6.4e-3
bool bsdf_is_effectively_delta(float alpha) 
{ 
	return alpha <= 0.0064; 
}

bool bsdf_is_effectively_delta(vec2 alpha) 
{ 
	return min(alpha.x, alpha.y) <= 0.0064; 
}

float SchlickR0(float eta)
{
	float val = (eta - 1.0) / (eta + 1.0);
	return val * val;
}

float disney_fresnel(vec3 wi, vec3 wo, float roughness, out float f_wi, out float f_wo)
 {
	vec3 h = normalize(wi + wo);
	float wo_dot_h = dot(wo, h);
	float fd90 = 0.5 + 2.0 * wo_dot_h * wo_dot_h * roughness;
	float fd0 = 1.f;
	f_wi = fresnel_schlick(fd0, fd90, wi.z);
	f_wo = fresnel_schlick(fd0, fd90, wo.z);
	return f_wi * f_wo;
}

float disney_fresnel(vec3 wi, vec3 wo, float roughness) {
	float unused1, unused2;
	return disney_fresnel(wi, wo, roughness, unused1, unused2);
}

vec3 calc_tint(vec3 albedo) 
{
	float lum = Luminance(albedo);
	return lum > 0 ? albedo / lum : vec3(1);
}


vec2 calc_anisotropy(float roughness, float anisotropic) 
{
	float aspect = sqrt(1.0 - 0.9 * anisotropic);
	float roughness_sqr = roughness * roughness;
	return vec2(max(0.001, roughness_sqr / aspect), max(0.001, roughness_sqr * aspect));
}

float Smith_lambda_isotropic(float alpha_sqr, float cos_theta) 
{
	if (cos_theta == 0) {
		return 0;
	}
	float cos_sqr = cos_theta * cos_theta;
	float tan_sqr = max(1.0 - cos_sqr, 0) / cos_sqr;
	return 0.5 * (sqrt(1.0 + alpha_sqr * tan_sqr) - 1);
}

float Smith_lambda_anisotropic(vec3 w, vec2 alpha) 
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

float G1_GGX_anisotropic(vec3 w, vec2 alpha) { 
    return 1.0 / (1.0 + Smith_lambda_anisotropic(w, alpha)); 
}

float G_GGX_correlated_isotropic(float alpha, vec3 wo, vec3 wi) {
	float alpha_sqr = alpha * alpha;
	return 1.0 / (1.0 + Smith_lambda_isotropic(alpha_sqr, wo.z) + Smith_lambda_isotropic(alpha_sqr, wi.z));
}

float G_GGX_correlated_anisotropic(vec2 alpha, vec3 wo, vec3 wi) {
	return 1.0 / (1.0 + Smith_lambda_anisotropic(wo, alpha) + Smith_lambda_anisotropic(wi, alpha));
}

float D_GGX_anisotropic(vec2 alpha, vec3 h) {
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

// Eq. 19 in https://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
float D_GGX_isotropic(float alpha_sqr, float cos_theta) {
	float d = ((cos_theta * alpha_sqr - cos_theta) * cos_theta + 1);
	return alpha_sqr / (d * d * Pi);
}

// Eq. 34 in https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
float G1_GGX_isotropic(float alpha_sqr, float cos_theta) {
	float cos_sqr = cos_theta * cos_theta;
	float tan_sqr = max(1.0 - cos_sqr, 0.0) / cos_sqr;
	return 2.0 / (1.0 + sqrt(1.0 + alpha_sqr * tan_sqr));
}

// pdf = G1(wo) * D(h) * max(0,dot(wo,h)) / wo.z
float eval_vndf_pdf_isotropic(float alpha, vec3 wo, vec3 h, out float D)
{
	D = 0.0;
	if (wo.z <= 0) {
		return 0.0;
	}
	float alpha_sqr = alpha * alpha;
	float G1 = G1_GGX_isotropic(alpha_sqr, wo.z);
	D = D_GGX_isotropic(alpha_sqr, h.z);
	return G1 * D * max(0.0, dot(wo, h)) / abs(wo.z);
}

float eval_vndf_pdf_isotropic(float alpha, vec3 wo, vec3 h)
 {
	float unused_D;
	return eval_vndf_pdf_isotropic(alpha, wo, h, unused_D); 
}

float eval_vndf_pdf_anisotropic(vec2 alpha, vec3 wo, vec3 h, out float D) {
	D = 0.0;
	if (wo.z <= 0) {
		return 0.0;
	}
	float G1 = G1_GGX_anisotropic(wo, alpha);
	D = D_GGX_anisotropic(alpha, h);
	return G1 * D * max(0.0, dot(wo, h)) / abs(wo.z);
}

float eval_vndf_pdf_anisotropic(vec2 alpha, vec3 wo, vec3 h)  
{
	float unused_D;
	return eval_vndf_pdf_anisotropic(alpha, wo, h, unused_D);
}  

float GGX_D(float a2, float cos_theta) {
	float d = ((cos_theta * a2 - cos_theta) * cos_theta + 1);
	return a2 / (d * d * Pi);
}

float GGX_G1(float a2, float cos_theta) {
	float cos2 = cos_theta * cos_theta;
	float tan2 = max(1.0 - cos2, 0.0) / cos2;
	return 2.0 / (1.0 + sqrt(1.0 + a2 * tan2));
}
#endif 