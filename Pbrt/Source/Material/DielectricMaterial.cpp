#include <Material/DielectricMaterial.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU DielectricMaterial::DielectricMaterial(SpectrumTexture reflectance, Float eta, Image* normalMap) :
		reflectance(reflectance), eta(eta), normalMap(normalMap), bxdf(eta) {}

	CPU_GPU RGBA DielectricMaterial::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		return reflectance.Evaluate(ctx, lambda);
	}

	CPU_GPU DielectricBxDF* DielectricMaterial::GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda) {
		return &bxdf;
	}

	CPU_GPU const Image* DielectricMaterial::GetNormalMap() const { return normalMap; }
}