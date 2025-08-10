#include <CookTorrance/TrowbridgeReitzDistribution.h>
#include <Math/Math.h>
#include <Math/Sampling.h>
#include <Type/VecMath.h>
namespace Render {
    CPU_GPU TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(Float alpha_x, Float alpha_y)
		: alpha_x(alpha_x), alpha_y(alpha_y) {}


    CPU_GPU bool TrowbridgeReitzDistribution::EffectivelySmooth() const {
        return std::max(alpha_x, alpha_y) < 1e-3f; 
    }

    CPU_GPU Float TrowbridgeReitzDistribution::D(Vector3f wm) const {
        Float tan2Theta = Tan2Theta(wm);
        if (IsInf(tan2Theta))
            return 0;
        Float cos4Theta = Sqr(Cos2Theta(wm));
        if (cos4Theta < 1e-16f)
            return 0;
        Float e = tan2Theta * (Sqr(CosPhi(wm) / alpha_x) + Sqr(SinPhi(wm) / alpha_y));
        return 1 / (Pi * alpha_x * alpha_y * cos4Theta * Sqr(1 + e));
	}

    CPU_GPU Float TrowbridgeReitzDistribution::Lambda(Vector3f w) const {
        Float tan2Theta = Tan2Theta(w);
        if (IsInf(tan2Theta))
            return 0;
        Float alpha2 = Sqr(CosPhi(w) * alpha_x) + Sqr(SinPhi(w) * alpha_y);
        return (sqrtf(1 + alpha2 * tan2Theta) - 1) / 2;
    }

    CPU_GPU Float TrowbridgeReitzDistribution::G1(Vector3f w) const {
        return 1 / (1 + Lambda(w));
    }

    CPU_GPU Float TrowbridgeReitzDistribution::G(Vector3f wo, Vector3f wi) const {
        return 1 / (1 + Lambda(wo) + Lambda(wi)); 
    }

    CPU_GPU Float TrowbridgeReitzDistribution::D(Vector3f w, Vector3f wm) const {
        return G1(w) / AbsCosTheta(w) * D(wm) * AbsDot(w, wm);
    }

    CPU_GPU Float TrowbridgeReitzDistribution::PDF(Vector3f w, Vector3f wm) const {
        return D(w, wm); 
    }

    CPU_GPU Vector3f TrowbridgeReitzDistribution::Sample_wm(Vector3f w, Point2f u) const {
        // Transform _w_ to hemispherical configuration
        Vector3f wh = Normalize(Vector3f(alpha_x * w.x, alpha_y * w.y, w.z));
        if (wh.z < 0)
            wh = -wh;

        // Find orthonormal basis for visible normal sampling
        Vector3f T1 = (wh.z < 0.99999f) ? Normalize(Cross(Vector3f(0, 0, 1), wh))
            : Vector3f(1, 0, 0);
        Vector3f T2 = Cross(wh, T1);

        // Generate uniformly distributed points on the unit disk
        Point2f p = SampleUniformDiskPolar(u);

        // Warp hemispherical projection for visible normal sampling
        Float h = sqrtf(1 - Sqr(p.x));
        p.y = Lerp((1 + wh.z) / 2, h, p.y);

        // Reproject to hemisphere and transform normal to ellipsoid configuration
        Float pz = sqrtf(max<Float>(0, 1 - LengthSquared(Vector2f(p))));
        Vector3f nh = p.x * T1 + p.y * T2 + pz * wh;

        return Normalize(
            Vector3f(alpha_x * nh.x, alpha_y * nh.y, max<Float>(1e-6f, nh.z)));
    }

    CPU_GPU void TrowbridgeReitzDistribution::Regularize() {
        if (alpha_x < 0.3f)
            alpha_x = Clamp(2 * alpha_x, 0.1f, 0.3f);
        if (alpha_y < 0.3f)
            alpha_y = Clamp(2 * alpha_y, 0.1f, 0.3f);
   }
}