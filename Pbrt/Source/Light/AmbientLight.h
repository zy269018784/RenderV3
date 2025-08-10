#pragma once
#include <Light/LightBase.h>
#include <Texture/RGB.h>
namespace Render {
    struct LightLiSample;
    struct SampledWavelengths;
    class LightSampleContext;

    class AmbientLight : public LightBase {
    public:
        AmbientLight(Transform worldFromLight, RGB L, Float power);

        static AmbientLight* Create(const Transform& worldFromLight, RGB L, Float power,  Allocator alloc);
       
        CPU_GPU LightLiSample SampleLi(
            LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
            bool allowIncompletePDF = false) const;
    private:
        RGB L;
        Float power; // ¡¡∂»
    };
}