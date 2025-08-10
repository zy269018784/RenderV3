#pragma once
#include <cpu_gpu.h>
#include <Texture/RGB.h>
namespace Render {
	struct SampledWavelengths;
	struct TextureEvalContext;
	class SpectrumNoiseTexture {
	public:
		SpectrumNoiseTexture() = default;
		SpectrumNoiseTexture(Float scale, Float freq) : scale(scale), freq(freq) {}
		CPU_GPU  struct RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
	private:
		Float freq;
		Float scale;
	};
}