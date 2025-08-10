#pragma once
#include <Light/LightBase.h>
#include <Texture/RGB.h>
namespace Render {
    struct LightLiSample;
    struct SampledWavelengths;
    class LightSampleContext;

    class DistantLight : public LightBase {
    public:
        DistantLight(Transform worldFromLight, RGB L, Float power, Point3f from, Point3f to);

        static DistantLight* Create(const Transform& worldFromLight, RGB L, Float power, 
                            Point3f from, Point3f to, 
                            Allocator alloc);
       
        CPU_GPU LightLiSample SampleLi(
            LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
            bool allowIncompletePDF = false) const;
    private:
        RGB L;
        Float power; // ¡¡∂»
        Point3f from;
        Point3f to;
    };
}