#include <Material/DiffuseMaterial.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU DiffuseMaterial::DiffuseMaterial(SpectrumTexture reflectance, Image* normalMap) 
		: reflectance(reflectance), normalMap(normalMap), bxdf() {}

	CPU_GPU RGBA DiffuseMaterial::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		return reflectance.Evaluate(ctx, lambda);
	}

	CPU_GPU DiffuseBxDF* DiffuseMaterial::GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda) {
		return &bxdf;
	}

	CPU_GPU const Image* DiffuseMaterial::GetNormalMap() const { return normalMap; }
}