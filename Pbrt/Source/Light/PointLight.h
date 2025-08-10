#pragma once
#include <Light/LightBase.h>
#include <Texture/RGB.h>
namespace Render {
    struct LightLiSample;
    struct SampledWavelengths;
    class LightSampleContext;

    class PointLight : public LightBase {
    public:
        PointLight(Transform worldFromLight, RGB L, Float power, Float range);

        static PointLight* Create(const Transform& worldFromLight,
            RGB L, Float power, Float range, Point3f from, Allocator alloc);

        CPU_GPU LightLiSample SampleLi(
            LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
            bool allowIncompletePDF = false) const;
    private:
        CPU_GPU Float Power(Float d) const;
    private:
        RGB L;
        Float power; // ¡¡∂»
        Float range; // À•ºı∑∂Œß
    };
}