#pragma once
#include <Material/Material.h>
#include <BxDF/BSDF.h>
#include <Type/Point3fi.h>
namespace Render {
    class Ray;
    class Sampler;
    class Camera;
    struct SampledWavelengths;

    class SurfaceInteraction {
    public:
        SurfaceInteraction() = default;
        CPU_GPU
            SurfaceInteraction(Point3fi pi) {
            this->pi = pi;
        }
        CPU_GPU
            SurfaceInteraction(Point3fi pi, Point2f uv, Vector3f wo,
                Normal3f g_n,
                Vector3f g_dpdu, Vector3f g_dpdv,
                Normal3f g_dndu, Normal3f g_dndv,
                Normal3f s_n,
                Vector3f s_dpdu, Vector3f s_dpdv,
                Normal3f s_dndu, Normal3f s_dndv,
                bool flipNormal) {

            this->pi = pi;
            this->uv = uv;
            this->wo = wo;

            if (flipNormal) {
                g_n *= -1;
                s_n *= -1;
            }

            geometry.n = g_n;
            geometry.dpdu = g_dpdu;
            geometry.dpdv = g_dpdv;
            geometry.dndu = g_dndu;
            geometry.dndv = g_dndv;

            shading.n = s_n;
            shading.dpdu = s_dpdu;
            shading.dpdv = s_dpdv;
            shading.dndu = s_dndu;
            shading.dndv = s_dndv;
        }
        CPU_GPU
            Point3f p() const { return Point3f(pi); }
        CPU_GPU
            BSDF GetBSDF();
    public:
        Point3fi pi;        // 交点
        Point2f uv;         // 交点的uv坐标
        Vector3f wo;        // 出射方向
        // 几何参数
        struct {
            Normal3f n;
            Vector3f dpdu, dpdv;
            Normal3f dndu, dndv;
        } geometry;
        // 着色参数
        struct {
            Normal3f n;
            Vector3f dpdu, dpdv;
            Normal3f dndu, dndv;
        } shading;
        Material material;                  // 材质
    };
}