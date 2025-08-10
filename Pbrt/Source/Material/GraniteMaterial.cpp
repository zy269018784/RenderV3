#include <Material/GraniteMaterial.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU GraniteMaterial::GraniteMaterial(SpectrumTexture reflectance, Float reflectivity, Float transmissivity, Image* normalMap) :
		reflectance(reflectance), transmissivity(transmissivity), reflectivity(reflectivity), normalMap(normalMap), bxdf(reflectivity, transmissivity) {}

	CPU_GPU RGBA GraniteMaterial::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		return reflectance.Evaluate(ctx, lambda);
	}

	CPU_GPU GraniteBxDF* GraniteMaterial::GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda) {
		return &bxdf;
	}

	CPU_GPU const Image* GraniteMaterial::GetNormalMap() const { return normalMap; }
}