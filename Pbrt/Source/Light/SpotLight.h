#pragma once
#include <Light/LightBase.h>
#include <Texture/RGB.h>
namespace Render {
    struct LightLiSample;
    struct SampledWavelengths;
    class LightSampleContext;

    class SpotLight : public LightBase {
    public:
        SpotLight(Transform worldFromLight, RGB L, Float power, Float range,
            Float totalWidth, Float falloffStart);

        static SpotLight* Create(const Transform& worldFromLight, RGB L, Float power, Float range,
                            Point3f from, Point3f to, 
                            Float coneangle,  Float conedelta ,
                            Allocator alloc);
       
        CPU_GPU LightLiSample SampleLi(
            LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
            bool allowIncompletePDF = false) const;
   private:
        CPU_GPU Float Power(Float d) const;
    private:
        RGB L;
        Float power; // ¡¡∂»
        Float cosFalloffStart;
        Float cosFalloffEnd;
        Float range; // À•ºı∑∂Œß
    };
}