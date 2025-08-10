#include <Intersection/SurfaceInteraction.h>
#include <Sampler/Sampler.h>
#include <Ray/Ray.h>
#include <Camera/Camera.h>
#include <Camera/SampledWavelengths.h>
#include <Texture/UniversalTextureEvaluator.h>
#include <Texture/MaterialEvalContext.h>
#include <Material/DiffuseMaterial.h>
#include <Memory/PolymorphicAllocator.h>
#include <Material/NormalMap.h>
namespace Render {
	//BSDF SurfaceInteraction::GetBSDF(const Ray& ray, SampledWavelengths& lambda, Camera camera, Sampler sampler) {
	CPU_GPU BSDF SurfaceInteraction::GetBSDF() {
		if (!material)
			return {};

		MaterialEvalContext ctx(*this);
		// 法线贴图
		const Image* normalMap = material.GetNormalMap();
		if (normalMap) {
			Vector3f dpdu, dpdv;
			NormalMap(*normalMap, *this, &dpdu, &dpdv);
			Normal3f ns(Normalize(Cross(dpdu, dpdv)));
			this->shading.n = ns;
			this->shading.dpdu = dpdu;
			this->shading.dpdv = dpdv;
			while (LengthSquared(this->shading.dpdu) > 1e16f ||
				LengthSquared(this->shading.dpdv) > 1e16f) {
				this->shading.dpdu /= 1e8f;
				this->shading.dpdv /= 1e8f;
			}
			//shading.n = ns;
			//shading.dpdu = dpdu;
			//shading.dpdv = dpdv;
			//shading.dndu = dndus;
			//shading.dndv = dndvs;
			//while (LengthSquared(shading.dpdu) > 1e16f ||
			//	LengthSquared(shading.dpdv) > 1e16f) {
			//	shading.dpdu /= 1e8f;
			//	shading.dpdv /= 1e8f;
			//}
		}

		SampledWavelengths w;

		BSDF bsdf = material.GetBSDF(*this, w);
		return bsdf;
	}
}