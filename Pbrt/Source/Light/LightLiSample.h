#pragma once
#include <Math/Math.h>
#include <Intersection/Intersection.h>
#include <Texture/RGB.h>
namespace Render {
    struct LightLiSample {
        // LightLiSample Public Methods
        LightLiSample() = default;
        CPU_GPU
            LightLiSample(const RGB& L, const RGB& power, Vector3f wi, Float pdf,
                const SurfaceInteraction& pLight)
            : L(L), power(power), wi(wi), pdf(pdf), pLight(pLight) {}

        RGB L;              // 采样的光颜色
        RGB power;
        Vector3f wi;        // 物体表面点到光源的方向
        Float pdf;          // 概率分布
        SurfaceInteraction pLight; // 光源上的点
    };

}