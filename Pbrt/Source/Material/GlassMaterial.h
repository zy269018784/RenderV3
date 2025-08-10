#pragma once
#include <Math/Math.h>
#include <Texture/SpectrumTexture.h>
#include <BxDF/GlassBxDF.h>
namespace Render {
    struct RGBA;
    struct TextureEvalContext;
    struct MaterialEvalContext;
    struct SampledWavelengths;
    class GlassBxDF;
    class SpectrumTexture;
    class Image;
    class GlassMaterial {  
    public:
        using BxDF = GlassBxDF;
        GlassMaterial() = default;
        CPU_GPU GlassMaterial(SpectrumTexture reflectance, Float reflectivity = 1.0f, Float transmissivity = 1.0f, Image* normalMap = nullptr);

        CPU_GPU RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
       
        CPU_GPU GlassBxDF *GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda);

        CPU_GPU const Image* GetNormalMap() const;
    private:
        SpectrumTexture reflectance;
        Float reflectivity;
        Float transmissivity;
        GlassBxDF bxdf;
        Image* normalMap;
    };

}