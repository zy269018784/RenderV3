#pragma once
#include <Type/TypeDeclaration.h>
#include <Type/Types.h>
#include <cpu_gpu.h>
namespace Render {
	class TrowbridgeReitzDistribution {
	public:
		// TrowbridgeReitzDistribution Public Methods
		TrowbridgeReitzDistribution() = default;
		CPU_GPU TrowbridgeReitzDistribution(Float alpha_x, Float alpha_y);

		CPU_GPU bool EffectivelySmooth() const;
		// 法线分布函数
		CPU_GPU Float D(Vector3f wm) const;

		CPU_GPU Float Lambda(Vector3f w) const;

		CPU_GPU Float G1(Vector3f w) const;
		// 几何函数
		CPU_GPU Float G(Vector3f wo, Vector3f wi) const;

		CPU_GPU Float D(Vector3f w, Vector3f wm) const;

		CPU_GPU Float PDF(Vector3f w, Vector3f wm) const;
		// 微平面法线
		CPU_GPU Vector3f Sample_wm(Vector3f w, Point2f u) const;

		CPU_GPU void Regularize();

		CPU_GPU static Float RoughnessToAlpha(Float roughness) { return std::sqrt(roughness); }
	private:
		// TrowbridgeReitzDistribution Private Members
		Float alpha_x, alpha_y;
	};
}