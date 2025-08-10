#include <Texture/SpectrumConstTexture.h>
#include <Texture/TextureEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU struct RGBA SpectrumConstTexture::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		return rgba / 255.f;
	}

	CPU_GPU RGB SpectrumConstTexture::Color() const {
		return rgba;
	}
}