#include <Material/GlassMaterial.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU GlassMaterial::GlassMaterial(SpectrumTexture reflectance, Float reflectivity, Float transmissivity, Image* normalMap) :
		reflectance(reflectance), transmissivity(transmissivity), reflectivity(reflectivity), normalMap(normalMap), bxdf(reflectivity, transmissivity) {}

	CPU_GPU RGBA GlassMaterial::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		return reflectance.Evaluate(ctx, lambda);
	}

	CPU_GPU GlassBxDF* GlassMaterial::GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda) {
		return &bxdf;
	}

	CPU_GPU const Image* GlassMaterial::GetNormalMap() const { return normalMap; }
}