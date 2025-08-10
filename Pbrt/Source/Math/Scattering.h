#pragma once
#include "cpu_gpu.h"
#include <Math/Math.h>
namespace Render {
	CPU_GPU inline Vector3f Reflect(Vector3f wo, Vector3f n) {
		return -wo + 2 * n * Dot(wo, n);
	}

    CPU_GPU inline bool Refract(Vector3f wi, Vector3f n, Float eta, Float* etap,
        Vector3f* wt) {
        Float cosTheta_i = Dot(n, wi);
        // Potentially flip interface orientation for Snell's law
        if (cosTheta_i < 0) {
            eta = 1 / eta;
            cosTheta_i = -cosTheta_i;
            n = -n;
        }

        // Compute $\cos\,\theta_\roman{t}$ using Snell's law
        Float sin2Theta_i = fmaxf(0, 1 - Sqr(cosTheta_i));
        Float sin2Theta_t = sin2Theta_i / Sqr(eta);
        // Handle total internal reflection case
        if (sin2Theta_t >= 1)
            return false;

        Float cosTheta_t = SafeSqrt(1 - sin2Theta_t);

        *wt = -wi / eta + Vector3f(n) * (cosTheta_i / eta - cosTheta_t);
        // Provide relative IOR along ray to caller
        if (etap)
            *etap = eta;

        return true;
    }

    CPU_GPU inline Float HenyeyGreenstein(Float g, Float cosTheta) {
        Float denom = 1 + Sqr(g) - 2 * g * cosTheta;
        return Inv4Pi * (1 - Sqr(g)) / (denom * SafeSqrt(denom));
    }

    // Fresnel Inline Functions
    CPU_GPU inline Float FrDielectric(Float cosTheta_i, Float eta) {
        cosTheta_i = Clamp(cosTheta_i, -1, 1);
        // Potentially flip interface orientation for Fresnel equations
        if (cosTheta_i < 0) {
            eta = 1 / eta;
            cosTheta_i = -cosTheta_i;
        }

        // Compute $\cos\,\theta_\roman{t}$ for Fresnel equations using Snell's law
        Float sin2Theta_i = 1 - Sqr(cosTheta_i);
        Float sin2Theta_t = sin2Theta_i / Sqr(eta);
        if (sin2Theta_t >= 1)
            return 1.f;
        Float cosTheta_t = SafeSqrt(1 - sin2Theta_t);

        Float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
        Float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
        return (Sqr(r_parl) + Sqr(r_perp)) / 2;
    }

    CPU_GPU inline Float FrComplex(Float cosTheta_i, complex<Float> eta) {
        using Complex = complex<Float>;
        cosTheta_i = Clamp(cosTheta_i, 0, 1);
        // Compute complex $\cos\,\theta_\roman{t}$ for Fresnel equations using Snell's law
        Float sin2Theta_i = 1 - Sqr(cosTheta_i);
        Complex sin2Theta_t = sin2Theta_i / Sqr(eta);
        Complex cosTheta_t = sqrt_complex(1 - sin2Theta_t);

        Complex r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
        Complex r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
        return (norm(r_parl) + norm(r_perp)) / 2;
    }
}