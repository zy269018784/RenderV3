#include <Texture/SpectrumTexture.h>
#include <Texture/SpectrumConstTexture.h>
#include <Texture/SpectrumImageTexture.h>
#include <Texture/SpectrumNoiseTexture.h>
#include <Texture/SpectrumRandomImageTexture.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/RGB.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU struct RGBA SpectrumTexture::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		auto eval = [&](auto ptr) { return ptr->Evaluate(ctx, lambda); };
		return Dispatch(eval);
	}
}