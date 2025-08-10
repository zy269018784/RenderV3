#pragma once
#include <Image/Image.h>
namespace Render {
	class MIPMap {
	public:
		MIPMap(Image image, WrapMode wrapMode,
			Allocator alloc);

		Image* GeneratePyramid(Image image, WrapMode2D wrapMode, Allocator alloc);

		static MIPMap* CreateFromFile(const std::string& filename, WrapMode wrapMode,Allocator *alloc = nullptr);
	private:
		WrapMode wrapMode;
	};
}