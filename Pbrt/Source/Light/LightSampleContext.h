#pragma once 
#include <Math/Math.h>
#include <Intersection/Intersection.h>
namespace Render {
    /*
        pi: 接受光照的点的世界坐标
        wi: pi到光源的方向
        n: 接受光照的点的法向量

    */
    class LightSampleContext {
    public:
        LightSampleContext() = default;
        CPU_GPU
            LightSampleContext(const SurfaceInteraction& si)
            : pi(si.pi), n(si.geometry.n), ns(si.shading.n) {}
        CPU_GPU
            LightSampleContext(const SurfaceInteraction& si, Vector3f wi)
            : pi(si.pi), n(si.geometry.n), ns(si.shading.n), wi(wi) {}
        CPU_GPU
            LightSampleContext(Point3fi pi, Normal3f n, Normal3f ns) : pi(pi), n(n), ns(ns) {}

        CPU_GPU
            Point3f p() const { return Point3f(pi); }

    public:
        Point3fi pi;
        Normal3f n, ns;
        Vector3f wi;
    };
}