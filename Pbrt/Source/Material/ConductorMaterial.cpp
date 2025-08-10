#include <Material/ConductorMaterial.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU ConductorMaterial::ConductorMaterial(SpectrumTexture reflectance, Float uRoughness, Float vRoughness, bool remaproughness, Image* normalMap)
		: reflectance(reflectance), uRoughness(uRoughness), vRoughness(vRoughness), remapRoughness(remaproughness), normalMap(normalMap), bxdf() {}

	CPU_GPU RGBA ConductorMaterial::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		return reflectance.Evaluate(ctx, lambda);
	}

	CPU_GPU ConductorBxDF* ConductorMaterial::GetBxDF(MaterialEvalContext ctx, SampledWavelengths lambda)  {
		Float uRough = uRoughness;
		Float vRough = vRoughness;
		if (remapRoughness) {
			uRough = TrowbridgeReitzDistribution::RoughnessToAlpha(uRough);
			vRough = TrowbridgeReitzDistribution::RoughnessToAlpha(vRough);
		}
		//printf("ConductorMaterial::GetBxDF 1\n");
		TrowbridgeReitzDistribution mfDistrib(uRough, vRough);
		//printf("ConductorMaterial::GetBxDF 2\n");
		new (&bxdf) ConductorBxDF(mfDistrib);
		//printf("ConductorMaterial::GetBxDF 3\n");
		return &bxdf;
	}

	CPU_GPU const Image* ConductorMaterial::GetNormalMap() const { return normalMap; }
}