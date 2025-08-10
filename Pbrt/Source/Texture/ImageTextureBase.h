#pragma once
#include <Image/Image.h>

namespace Render {
	class ImageTextureBase {
	public:
		ImageTextureBase(Image* image) : image(image) {
			
		}
	public:
		Image* image = nullptr;
	};
}