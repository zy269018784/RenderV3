#pragma once
#include <cpu_gpu.h>
#include <Math/Math.h>
#include <Math/Sampling.h>
#include <Math/Float.h>
namespace Render {
    CPU_GPU inline Float AnisotropicGGX(Float alpha_x, Float alpha_y, Vector3f wm)  {
        Float tan2Theta = Tan2Theta(wm);
        if (IsInf(tan2Theta))
            return 0;
        Float cos4Theta = Sqr(Cos2Theta(wm));
        if (cos4Theta < 1e-16f)
            return 0;
        Float e = tan2Theta * (Sqr(CosPhi(wm) / alpha_x) + Sqr(SinPhi(wm) / alpha_y));
        return 1 / (Pi * alpha_x * alpha_y * cos4Theta * Sqr(1 + e));
    }

    CPU_GPU
        Vector3f inline Sample_wm(Vector3f w, Point2f u, Float alpha_x, Float alpha_y) {
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
        Float h = std::sqrt(1 - Sqr(p.x));
        p.y = Lerp((1 + wh.z) / 2, h, p.y);

        // Reproject to hemisphere and transform normal to ellipsoid configuration
        Float pz = std::sqrt(std::max<Float>(0, 1 - LengthSquared(Vector2f(p))));
        Vector3f nh = p.x * T1 + p.y * T2 + pz * wh;
        return Normalize(
            Vector3f(alpha_x * nh.x, alpha_y * nh.y, std::max<Float>(1e-6f, nh.z)));
    }
}