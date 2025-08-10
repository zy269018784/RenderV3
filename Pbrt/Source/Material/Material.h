#pragma once
#include <Util/TaggedPointer.h>
namespace Render {
    struct MaterialEvalContext;
    struct TextureEvalContext;
    struct SampledWavelengths;
    struct RGBA;
    class UniversalTextureEvaluator;
    class BSDF;
    class BxDF;
    class DiffuseMaterial;
    class DielectricMaterial;
    class ConductorMaterial;
    class GlassMaterial;
    class MirrorMaterial;
    class GraniteMaterial;
    class Image;
    class Material: public TaggedPointer<DiffuseMaterial, ConductorMaterial, GlassMaterial, DielectricMaterial, MirrorMaterial, GraniteMaterial> {
    public:
        using TaggedPointer::TaggedPointer;
        CPU_GPU RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;

        CPU_GPU BSDF GetBSDF(MaterialEvalContext ctx, SampledWavelengths& lambda);

        CPU_GPU const Image* GetNormalMap() const;
    };
}  
