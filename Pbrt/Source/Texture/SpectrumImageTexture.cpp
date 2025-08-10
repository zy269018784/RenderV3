#include <Texture/SpectrumImageTexture.h>
#include <Texture/TextureEvalContext.h>
#include <Camera/SampledWavelengths.h>
namespace Render {
	CPU_GPU struct RGBA SpectrumImageTexture::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		if (image) {
			// 图像分辨率
			Point2i res = image->Resolution();

			//printf("uv %f %f\n", ctx.uv[0], ctx.uv[1]);
			// t: 0 对应图像的最后一行, 1 对应图像的第一行.
			ctx.uv[1] = 1 - ctx.uv[1];

			// 像素坐标
			//printf("uv %f %f\n", ctx.uv[0], ctx.uv[1]);
			//Point2i pixel(ctx.uv[0] * res[0], ctx.uv[1] * res[1]);

			RGBA rgb;
			rgb.r = image->BilerpChannel(ctx.uv, 0, WrapMode::Repeat);
			rgb.g = image->BilerpChannel(ctx.uv, 1, WrapMode::Repeat);
			rgb.b = image->BilerpChannel(ctx.uv, 2, WrapMode::Repeat);
			rgb.a = image->BilerpChannel(ctx.uv, 3, WrapMode::Repeat);
			rgb.r /= 255.f;
			rgb.g /= 255.f;
			rgb.b /= 255.f;
			rgb.a /= 255.f;
			return rgb;
		}
		return RGBA();
	}
}