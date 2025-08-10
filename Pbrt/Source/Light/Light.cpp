#include <Light/Light.h>
#include <Light/PointLight.h>
#include <Light/SpotLight.h>
#include <Light/DistantLight.h>
#include <Light/AmbientLight.h>
#include <Light/HDRLight.h>

#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
    CPU_GPU LightLiSample Light::SampleLi(
        LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
        bool allowIncompletePDF) const {
        auto sample = [&](auto ptr) {
            return ptr->SampleLi(ctx, u, lambda, allowIncompletePDF);
        };
        return Dispatch(sample);
    }
}