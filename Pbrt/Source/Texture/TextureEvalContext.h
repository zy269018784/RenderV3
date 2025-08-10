#pragma once
#include <cpu_gpu.h>
#include <Intersection/SurfaceInteraction.h>
#include <Intersection/Interaction.h>
namespace Render {
    struct TextureEvalContext {
        TextureEvalContext() = default;
        CPU_GPU
            TextureEvalContext(const Interaction& intr) : p(intr.p()), uv(intr.uv) {}
        CPU_GPU
            TextureEvalContext(const SurfaceInteraction& si)
            : p(si.p()), n(si.shading.n), uv(si.uv) {}
        CPU_GPU
            TextureEvalContext(Point3f p, Normal3f n, Point2f uv)
            : p(p), n(n), uv(uv) {}
        Point3f p;              // 交点
        Normal3f n;             // 法线
        Point2f uv;             // 纹理UV
    };
}