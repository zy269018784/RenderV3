#pragma once
#include <Math/Math.h>
#include <Texture/SpectrumTexture.h>
#include <BxDF/DiffuseBxDF.h>
namespace Render {
    struct RGBA;
    struct TextureEvalContext;
    struct SampledWavelengths;
    struct MaterialEvalContext;
    class DiffuseBxDF;
    class SpectrumTexture;
    class Image;
    class DiffuseMaterial {  
    public:
        using BxDF = DiffuseBxDF;
        DiffuseMaterial() = default;
        CPU_GPU DiffuseMaterial(SpectrumTexture reflectance, Image* normalMap = nullptr);

        CPU_GPU RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
       
        CPU_GPU DiffuseBxDF *GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda);

        CPU_GPU const Image* GetNormalMap() const;
    private:
        SpectrumTexture reflectance;
        DiffuseBxDF bxdf;
        Image* normalMap;
    };

}