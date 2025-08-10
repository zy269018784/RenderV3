#pragma once
#include <cpu_gpu.h>
#include <Texture/RGB.h>
namespace Render {
	struct SampledWavelengths;
	struct TextureEvalContext;
	class SpectrumConstTexture  {
	public:
		SpectrumConstTexture(RGB value) : rgba(value, 1.0) {}
		SpectrumConstTexture(RGBA value) : rgba(value) {}
		CPU_GPU  struct RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
		CPU_GPU RGB Color() const;
	private:
		RGBA rgba;
	};
}