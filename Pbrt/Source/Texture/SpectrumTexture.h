#pragma once
#include <cpu_gpu.h>
#include <Util/TaggedPointer.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	class SpectrumConstTexture;
	class SpectrumImageTexture;
	class SpectrumNoiseTexture;
	class SpectrumRandomImageTexture;
	struct TextureEvalContext;
	struct RGBA;
	class SpectrumTexture : public TaggedPointer<SpectrumImageTexture, SpectrumConstTexture, SpectrumNoiseTexture, SpectrumRandomImageTexture> {
	public:
		using TaggedPointer::TaggedPointer;
		CPU_GPU struct RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
	};
}
