#if 0
vec3 cosineSampleHemisphere()
{ 
	float r1 = rnd(seed);
	float r2 = rnd(seed);

	vec3 dir;
	float r = sqrt(r1);
	float phi = 2.0 * M_PI * r2;

	dir.x = r * cos(phi);
	dir.y = r * sin(phi);
	dir.z = sqrt(max(0.0, 1.0 - dir.x*dir.x - dir.y*dir.y));

	return dir;
}

/*
 * Calculates local coordinate frame for a given normal
 */
mat3 localFrame(in vec3 normal)
{
	vec3 up       = abs(normal.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
	vec3 tangentX = normalize(cross(up, normal));
	vec3 tangentY = cross(normal, tangentX);
	return mat3(tangentX, tangentY, normal);
}

vec3 ImportanceSampleGTR1(float rgh, float r1, float r2)
{
	float a = max(0.001, rgh);
	float a2 = a * a; 
	float phi = r1 * TWO_PI; 
	float cosTheta = sqrt((1.0 - pow(a2, 1.0 - r1)) / (1.0 - a2));
	float sinTheta = clamp(sqrt(1.0 - (cosTheta * cosTheta)), 0.0, 1.0);
	float sinPhi = sin(phi);
	float cosPhi = cos(phi); 
	return vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
}

vec3 ImportanceSampleGTR2_aniso(float ax, float ay, float r1, float r2)
{
	float phi = r1 * TWO_PI; 
	float sinPhi = ay * sin(phi);
	float cosPhi = ax * cos(phi);
	float tanTheta = sqrt(r2 / (1 - r2)); 
	return vec3(tanTheta * cosPhi, tanTheta * sinPhi, 1.0);
}

vec3 ImportanceSampleGTR2(float rgh, float r1, float r2)
{
	float a = max(0.001, rgh); 
	float phi = r1 * TWO_PI; 
	float cosTheta = sqrt((1.0 - r2) / (1.0 + (a * a - 1.0) * r2));
	float sinTheta = clamp(sqrt(1.0 - (cosTheta * cosTheta)), 0.0, 1.0);
	float sinPhi = sin(phi);
	float cosPhi = cos(phi); 
	return vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
}

 /*
  * The masking shadowing function Smith for GGX noraml distribution (G)
  */
float SmithGGX(float NDotv, float alphaG)	
{
	float a = alphaG * alphaG;
	float b = NDotv * NDotv;
	return 1.0 / (NDotv + sqrt(a + b - a * b));
}

 /*
  * Schlick's approximation of the specular reflection coefficient R
  * (1 - cosTheta)^5
  */
float SchlickFresnel(float u)
{
	float m = clamp(1.0 - u, 0.0, 1.0);
	return m * m * m * m * m; // power of 5
}

float DielectricFresnel(float cos_theta_i, float eta)
{
	float sinThetaTSq = eta * eta * (1.0f - cos_theta_i * cos_theta_i); 
	// Total internal reflection
	if (sinThetaTSq > 1.0)
		return 1.0;

	float cos_theta_t = sqrt(max(1.0 - sinThetaTSq, 0.0)); 
	float rs = (eta * cos_theta_t - cos_theta_i) / (eta * cos_theta_t + cos_theta_i);
	float rp = (eta * cos_theta_i - cos_theta_t) / (eta * cos_theta_i + cos_theta_t); 
	return 0.5f * (rs * rs + rp * rp);
}

/*
 * Generalized-Trowbridge-Reitz (D)
 */
float GTR1(float NDotH, float a)
{
	if (a >= 1.0)
		return (1.0 / M_PI); 
	float a2 = a * a;
	float t = 1.0 + (a2 - 1.0) * NDotH * NDotH; 
	return (a2 - 1.0) / (M_PI * log(a2) * t);
}

 /*
  * Generalized-Trowbridge-Reitz (D)
  * Describes differential area of microfacets for the surface normal
  */
float GTR2(float NDotH, float a)
{
	float a2 = a * a;
	float t = 1.0 + (a2 - 1.0) * NDotH * NDotH;
	return a2 / (M_PI * t * t);
}

vec3 EvalDielectricReflection(in Material material, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    pdf = 0.0;
    float NoL = dot(N,L);
    if (NoL <= 0.0) return vec3(0.0);  

    float eta = payload.eta; 
    float NoH = dot(N,H);
    float VoH = dot(V,H);

    float F = DielectricFresnel(VoH, eta);
    float D = GTR2(NoH, material.roughness); 
    pdf = D * NoH * F / (4.0 * abs(VoH)); 
    //float G = SmithGGX(abs(NoL), material.roughness) * SmithGGX(abs(dot(N, V)), material.roughness); 
	float G = 0;
    return material.p3.xyz * F * D * G;
}
 
vec3 EvalDielectricRefraction(in Material material, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{ 
    pdf = 0.0;
    float NoL = dot(N,L);
    if (NoL >= 0.0) return vec3(0.0);  

    float eta = payload.eta; 
    float NoH = dot(N,H);
    float VoH = dot(V,H);
    float LoH = dot(L,H);

    float F = DielectricFresnel(abs(VoH), eta);
    float D = GTR2(NoH, material.roughness); 
    float denomSqrt = LoH + VoH * eta;
    pdf = D * NoH * (1.0 - F) * abs(LoH) / (denomSqrt * denomSqrt); 
    //float G = SmithGGX(abs(NoL), material.roughness) * SmithGGX(abs(dot(N, V)), material.roughness); 
	float G = 0;
    return material.albedo.xyz * (1.0 - F) * D * G * abs(VoH) * abs(LoH) * 4.0 * eta * eta / (denomSqrt * denomSqrt);
} 

vec3 EvalDiffuse(in Material material, in vec3 Csheen, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    pdf = 0.0;
    float NoL = dot(N,L);
    if (NoL <= 0.0)
        return vec3(0.0);

    pdf = NoL * (1.0 / M_PI);

    float NoV = dot(N,V);
    float LoH = dot(L,H);

    // Diffuse
    float FL = SchlickFresnel(NoL);
    float FV = SchlickFresnel(NoV);
    float FH = SchlickFresnel(LoH);
    float Fd90 = 0.5 + 2.0 * LoH * LoH * material.roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);

    // Fake Subsurface TODO: Replace with volumetric scattering
    float Fss90 = LoH * LoH * material.roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss = 1.25 * (Fss * (1.0 / (NoL + NoV) - 0.5) + 0.5);

    vec3 Fsheen = FH * material.sheen * Csheen; 
    return ((1.0 / M_PI) * mix(Fd, ss, material.subsurface) * material.albedo.xyz + Fsheen) * (1.0 - material.metallic);
}

vec3 EvalSpecular(in Material material, in vec3 Cspec0, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    pdf = 0.0;
    float NoL = dot(N,L);
    if (NoL <= 0.0)
        return vec3(0.0);

    float NoH = dot(N,H); 
    float D = GTR2(NoH, material.roughness);
    pdf = D * NoH / (4.0 * dot(V, H)); 
    float FH = SchlickFresnel(dot(L, H));
    vec3 F = mix(Cspec0, vec3(1.0), FH);
    float G = SmithGGX(abs(NoL), material.roughness) * SmithGGX(abs(dot(N, V)), material.roughness); 
    return F * D * G;
}

vec3 EvalClearcoat(in Material material, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    pdf = 0.0;
    float NoL = dot(N,L);
    if (NoL <= 0.0)
        return vec3(0.0);

    float NoH = dot(N,H);
    float D = GTR1(NoH, mix(0.1, 0.001, material.clearcoatGloss));
    pdf = D * NoH / (4.0 * dot(V, H));

    float FH = SchlickFresnel(dot(L, H));
    float F = mix(0.04, 1.0, FH);
    float G = SmithGGX(NoL, 0.25) * SmithGGX(dot(N, V), 0.25); 
    return vec3(0.25 * material.clearcoat * F * D * G);
}

vec3 DisneySample(in Material material, inout vec3 L, inout float pdf)
{
    pdf = 0.0;
    vec3 f = vec3(0.0);

    float diffuseRatio = 0.5 * (1.0 - material.metallic);
    float transWeight = (1.0 - material.metallic) * material.transmission;

    vec3 Cdlin = material.albedo.xyz;
    float Cdlum = 0.3 * Cdlin.x + 0.6 * Cdlin.y + 0.1 * Cdlin.z; // luminance approx.

    vec3 Ctint = Cdlum > 0.0 ? Cdlin / Cdlum : vec3(1.0f); // normalize lum. to isolate hue+sat
    vec3 Cspec0 = mix(material.albedo.w * 0.08 * mix(vec3(1.0), Ctint, material.specularTint), Cdlin, material.metallic);
    vec3 Csheen = mix(vec3(1.0), Ctint, material.sheenTint);
    float eta = payload.eta;

    vec3 N = payload.ffnormal;
    vec3 V = -gl_WorldRayDirectionEXT;

    mat3 frame = localFrame(N);
     
    float r1 = rnd(seed);
    float r2 = rnd(seed);

    if (rnd(seed) < transWeight) {
        vec3 H = ImportanceSampleGTR2(material.roughness, r1, r2);
        H = frame * H;

        if (dot(V, H) < 0.0)
            H = -H;

        vec3 R = reflect(-V, H);
        float F = DielectricFresnel(abs(dot(R, H)), eta);
         
        if (r2 < F) {   // Reflection/Total internal reflection
            L = normalize(R);
            f = EvalDielectricReflection(material, V, N, L, H, pdf);
        }
        else {  // Transmission 
            L = normalize(refract(-V, H, eta));
            f = EvalDielectricRefraction(material, V, N, L, H, pdf);
        }

        f *= transWeight;
        pdf *= transWeight;
    }
    else
    {
        if (rnd(seed) < diffuseRatio) {
            L = cosineSampleHemisphere();
            L = frame * L; 
            vec3 H = normalize(L + V); 
            f = EvalDiffuse(material, Csheen, V, N, L, H, pdf);
            pdf *= diffuseRatio;
        }
        else {// Specular 
            float primarySpecRatio = 1.0 / (1.0 + material.clearcoat); 
            // Sample primary specular lobe
            if (rnd(seed) < primarySpecRatio) {
                // TODO: Implement http://jcgt.org/published/0007/04/01/
                vec3 H = ImportanceSampleGTR2(material.roughness, r1, r2);
                H = frame * H; 
                if (dot(V, H) < 0.0)
                    H = -H; 
                L = normalize(reflect(-V, H)); 
                f = EvalSpecular(material, Cspec0, V, N, L, H, pdf);
                pdf *= primarySpecRatio * (1.0 - diffuseRatio);
            }
            else {// Sample clearcoat lobe 
                vec3 H = ImportanceSampleGTR1(mix(0.1, 0.001, material.clearcoatGloss), r1, r2);
                H = frame * H; 
                if (dot(V, H) < 0.0)
                    H = -H; 
                L = normalize(reflect(-V, H)); 
                f = EvalClearcoat(material, V, N, L, H, pdf);
                pdf *= (1.0 - primarySpecRatio) * (1.0 - diffuseRatio);
            }
        } 
        f *= (1.0 - transWeight);
        pdf *= (1.0 - transWeight);
    } 
    return f;
}
    
vec3 DisneyEval(Material material, vec3 L, inout float pdf)
{
    vec3 N = payload.ffnormal;
    vec3 V = -gl_WorldRayDirectionEXT;
    float eta = payload.eta;

    vec3 H;
    bool refl = dot(N, L) > 0.0;

    if (refl)
        H = normalize(L + V);
    else
        H = normalize(L + V * eta);

    if (dot(V, H) < 0.0)
        H = -H;

    float diffuseRatio = 0.5 * (1.0 - material.metallic);
    float primarySpecRatio = 1.0 / (1.0 + material.clearcoat);
    float transWeight = (1.0 - material.metallic) * material.transmission;

    vec3 brdf = vec3(0.0);
    vec3 bsdf = vec3(0.0);
    float brdfPdf = 0.0;
    float bsdfPdf = 0.0;

    if (transWeight > 0.0) { 
        if (refl) {// Reflection
            bsdf = EvalDielectricReflection(material, V, N, L, H, bsdfPdf);  
        }
        else {// Transmission 
            bsdf = EvalDielectricRefraction(material, V, N, L, H, bsdfPdf);
        }
    } 

    float m_pdf; 
    if (transWeight < 1.0) {
        vec3 Cdlin = material.albedo.xyz;
        float Cdlum = 0.3 * Cdlin.x + 0.6 * Cdlin.y + 0.1 * Cdlin.z; // luminance approx.

        vec3 Ctint = Cdlum > 0.0 ? Cdlin / Cdlum : vec3(1.0f); // normalize lum. to isolate hue+sat
        vec3 Cspec0 = mix(material.albedo.w * 0.08 * mix(vec3(1.0), Ctint, material.specularTint), Cdlin, material.metallic);
        vec3 Csheen = mix(vec3(1.0), Ctint, material.sheenTint);

        // Diffuse
        brdf += EvalDiffuse(material, Csheen, V, N, L, H, m_pdf);
        brdfPdf += m_pdf * diffuseRatio;

        // Specular
        brdf += EvalSpecular(material, Cspec0, V, N, L, H, m_pdf);
        brdfPdf += m_pdf * primarySpecRatio * (1.0 - diffuseRatio);

        // Clearcoat
        brdf += EvalClearcoat(material, V, N, L, H, m_pdf);
        brdfPdf += m_pdf * (1.0 - primarySpecRatio) * (1.0 - diffuseRatio);
    } 
    pdf = mix(brdfPdf, bsdfPdf, transWeight); 
    return mix(brdf, bsdf, transWeight);
}
#endif