#include <Texture/SpectrumRandomImageTexture.h>
#include <Texture/TextureEvalContext.h>
#include <Camera/SampledWavelengths.h>
#include <Math/Transform.h>
namespace Render {
	CPU_GPU struct RGBA SpectrumRandomImageTexture::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
		
		if (image != nullptr) {
			// 图像分辨率
			Point2i res = image->Resolution();

			// t: 0 对应图像的最后一行, 1 对应图像的第一行.
			ctx.uv[1] = 1 - ctx.uv[1];


			Transform t({ 0, -1, 0, 0,
						  1,  0, 0, 0,
						  0,  0, 1, 0,
						  0,  0, 0, 1
				});
			Vector3f uv(ctx.uv[0] + 0.5f, ctx.uv[1], 0);

			RGBA rgb;
			//return rgb;
			// 像素坐标
			Point2i pixel(ctx.uv[0] * res[0], ctx.uv[1] * res[1]);
			rgb.r = image->GetChannel(pixel, 0, WrapMode::Repeat);
			rgb.g = image->GetChannel(pixel, 1, WrapMode::Repeat);
			rgb.b = image->GetChannel(pixel, 2, WrapMode::Repeat);
		
			uv = t(uv);

			Point2i pixel2(uv.x * res[0], uv.y * res[1]);
			rgb.r += image->GetChannel(pixel2, 0, WrapMode::Repeat);
			rgb.g += image->GetChannel(pixel2, 1, WrapMode::Repeat);
			rgb.b += image->GetChannel(pixel2, 2, WrapMode::Repeat);
			
			rgb *= 0.5f;
			rgb.a = 1.0;
			return rgb / 255.f;
		}
		return RGBA();
	}
}