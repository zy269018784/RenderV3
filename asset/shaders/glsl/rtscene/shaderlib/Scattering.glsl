#ifndef SCATTERING_H
#define SCATTERING_H
#include "Complex.glsl"
#include "Math.glsl"


vec3 Reflect(vec3 wo, vec3 n) 
{
    vec3 r = -wo + 2 * n * dot(wo, n);
    return r;
}

float FrComplex(float cosTheta_i, Complex eta) 
{
     cosTheta_i = clamp(cosTheta_i, 0, 1);
     // Compute Complex $\cos\,\theta_\roman{t}$ for Fresnel equations using Snell's law
     float sin2Theta_i = 1 - cosTheta_i * cosTheta_i;

    Complex eta_sqr = Mul(eta, eta);
    Complex sin2Theta_t = Div(sin2Theta_i, eta_sqr);

     Complex cosTheta_t = sqrt_Complex(Sub(1, sin2Theta_t));
     Complex a = Sub(Mul(cosTheta_i, eta), cosTheta_t);
     Complex b = Add(Mul(cosTheta_i, eta), cosTheta_t);
     Complex c = Sub(cosTheta_i, Mul(cosTheta_t, eta));
     Complex d = Add(cosTheta_i, Mul(cosTheta_t, eta));
     Complex r_parl = Div(a, b);
     Complex r_perp = Div(c, d);
     //Complex r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
     //Complex r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
     return (norm(r_parl) + norm(r_perp)) / 2;
}

float FrDielectric(float cosTheta_i, float eta) 
{
    cosTheta_i = Clamp(cosTheta_i, -1, 1);
    if (cosTheta_i < 0) {
         eta = 1 / eta;
         cosTheta_i = -cosTheta_i;
    }
    float sin2Theta_i = 1 - Sqr(cosTheta_i);
    float sin2Theta_t = sin2Theta_i / Sqr(eta);
    if (sin2Theta_t >= 1)
        return 1.f;
    float cosTheta_t = SafeSqrt(1 - sin2Theta_t);
    float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
    float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
    return (Sqr(r_parl) + Sqr(r_perp)) / 2;
}

#endif