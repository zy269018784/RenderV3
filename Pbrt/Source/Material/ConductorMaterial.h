#pragma once
#include <Math/Math.h>
#include <Texture/SpectrumTexture.h>
#include <BxDF/ConductorBxDF.h>
namespace Render {
    struct RGBA;
    struct TextureEvalContext;
    struct SampledWavelengths;
    struct MaterialEvalContext;
    class ConductorBxDF;
    class SpectrumTexture;
    class Image;
    class ConductorMaterial {  
    public:
        using BxDF = ConductorBxDF;
        ConductorMaterial() = default;
        CPU_GPU ConductorMaterial(SpectrumTexture reflectance, Float uRoughness, Float vRoughness, bool remapRoughness, Image* normalMap = nullptr);

        CPU_GPU  RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
       
        CPU_GPU ConductorBxDF *GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda);

        CPU_GPU const Image* GetNormalMap() const;
    private:
        SpectrumTexture reflectance;
        Float uRoughness, vRoughness;
        bool remapRoughness;
        ConductorBxDF bxdf;
        Image* normalMap;
    };

}