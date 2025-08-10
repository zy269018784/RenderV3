#pragma once
#include <Math/Math.h>
#include <Texture/SpectrumTexture.h>
#include <BxDF/DielectricBxDF.h>
namespace Render {
    struct RGBA;
    struct TextureEvalContext;
    struct MaterialEvalContext;
    struct SampledWavelengths;
    class DielectricBxDF;
    class SpectrumTexture;
    class Image;
    class DielectricMaterial {  
    public:
        using BxDF = DielectricBxDF;
        DielectricMaterial() = default;
        CPU_GPU DielectricMaterial(SpectrumTexture reflectance, Float eta, Image* normalMap = nullptr);

        CPU_GPU RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
       
        CPU_GPU DielectricBxDF *GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda);

        CPU_GPU const Image* GetNormalMap() const;
    private:
        SpectrumTexture reflectance;
        Float eta;
        DielectricBxDF bxdf;
        Image* normalMap;
    };

}