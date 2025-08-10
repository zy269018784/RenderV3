#include <Texture/UniversalTextureEvaluator.h>
#include <Texture/RGB.h>
#include <Texture/SpectrumTexture.h>
#include <Texture/TextureEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
    RGB UniversalTextureEvaluator::operator()(SpectrumTexture tex,
        TextureEvalContext ctx,
        SampledWavelengths lambda) {
        return RGB(tex.Evaluate(ctx, lambda));
    }
}