#include <Material/MirrorMaterial.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU MirrorMaterial::MirrorMaterial(SpectrumTexture reflectance, Image* normalMap)
		: reflectance(reflectance), normalMap(normalMap), bxdf() {}

	CPU_GPU RGBA MirrorMaterial::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		return reflectance.Evaluate(ctx, lambda);
	}

	CPU_GPU MirrorBxDF* MirrorMaterial::GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda) {
		return &bxdf;
	}

	CPU_GPU const Image* MirrorMaterial::GetNormalMap() const { return normalMap; }
}