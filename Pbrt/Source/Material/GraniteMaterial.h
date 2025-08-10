#pragma once
#include <Math/Math.h>
#include <Texture/SpectrumTexture.h>
#include <BxDF/GraniteBxDF.h>
namespace Render {
    struct RGBA;
    struct TextureEvalContext;
    struct MaterialEvalContext;
    struct SampledWavelengths;
    class GraniteBxDF;
    class SpectrumTexture;
    class Image;
    class GraniteMaterial {  
    public:
        using BxDF = GraniteBxDF;
        GraniteMaterial() = default;
        CPU_GPU GraniteMaterial(SpectrumTexture reflectance, Float reflectivity = 1.0f, Float transmissivity = 1.0f, Image* normalMap = nullptr);

        CPU_GPU RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
       
        CPU_GPU GraniteBxDF *GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda);

        CPU_GPU const Image* GetNormalMap() const;
    private:
        SpectrumTexture reflectance;
        Float reflectivity;
        Float transmissivity;
        GraniteBxDF bxdf;
        Image* normalMap;
    };

}