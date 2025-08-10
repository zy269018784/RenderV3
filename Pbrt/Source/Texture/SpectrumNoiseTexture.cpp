#include <Texture/SpectrumNoiseTexture.h>
#include <Texture/TextureEvalContext.h>
#include <Camera/SampledWavelengths.h>
#include <Math/Noise.h>
namespace Render {
	CPU_GPU struct RGBA SpectrumNoiseTexture::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		Vector3f uv(ctx.uv.x, ctx.uv.y, 0);
		Float Nosie = worleyNoise(uv * scale, freq);
		return RGBA(Nosie, Nosie, Nosie, Nosie);
	}
}