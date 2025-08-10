#pragma once
#include <Texture/ImageTextureBase.h>
#include <Texture/RGB.h>
namespace Render {
	struct SampledWavelengths;
	struct TextureEvalContext;
	class SpectrumImageTexture : public ImageTextureBase {
	public:
		SpectrumImageTexture(Image* image) : ImageTextureBase(image) {}
		CPU_GPU struct RGBA Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
	};
}