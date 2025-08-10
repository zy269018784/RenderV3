#pragma once
#include <cpu_gpu.h>
#include <initializer_list>
namespace Render {
    struct TextureEvalContext;
    struct SampledWavelengths;
    struct RGB;
    class SpectrumTexture;
    class UniversalTextureEvaluator {
    public:
        CPU_GPU
            bool CanEvaluate(std::initializer_list<SpectrumTexture>) const {
            return true;
        }
        CPU_GPU
            RGB operator()(SpectrumTexture tex, TextureEvalContext ctx,
                SampledWavelengths lambda);
    };
}