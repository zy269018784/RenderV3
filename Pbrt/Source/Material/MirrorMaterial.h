#pragma once
#include <Math/Math.h>
#include <Texture/SpectrumTexture.h>
#include <BxDF/MirrorBxDF.h>
namespace Render {
    struct RGBA;
    struct TextureEvalContext;
    struct SampledWavelengths;
    struct MaterialEvalContext;
    class MirrorBxDF;
    class SpectrumTexture;
    class Image;
    class MirrorMaterial {  
    public:
        using BxDF = MirrorBxDF;
        MirrorMaterial() = default;
        CPU_GPU MirrorMaterial(SpectrumTexture reflectance, Image* normalMap = nullptr);

        CPU_GPU RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
       
        CPU_GPU MirrorBxDF *GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda);

        CPU_GPU const Image* GetNormalMap() const;
    private:
        SpectrumTexture reflectance;
        MirrorBxDF bxdf;
        Image* normalMap;
    };

}