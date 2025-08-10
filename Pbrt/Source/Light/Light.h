#pragma once
#include <Util/TaggedPointer.h>
#include <Type/Types.h>
namespace Render {
    struct LightLiSample;
    struct SampledWavelengths;
    class LightSampleContext;
    class PointLight;
    class SpotLight;
    class DistantLight;
    class AmbientLight;
    class HDRLight;
    class Light
        : public TaggedPointer<PointLight, SpotLight, DistantLight, AmbientLight, HDRLight> {
    public:
        using TaggedPointer::TaggedPointer;

        CPU_GPU LightLiSample SampleLi(
            LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
            bool allowIncompletePDF = false) const;
    };
}