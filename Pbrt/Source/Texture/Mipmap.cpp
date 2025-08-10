#include <Texture/Mipmap.h>

namespace Render {
	MIPMap::MIPMap(Image image, WrapMode wrapMode,
		Allocator alloc)  : wrapMode(wrapMode) {
	
	}

	Image* MIPMap::GeneratePyramid(Image image, WrapMode2D wrapMode, Allocator alloc) {
		return nullptr;
	}

	MIPMap* MIPMap::CreateFromFile(const std::string& filename, WrapMode wrapMode, Allocator *alloc) {
		Image* image = Image::Read(filename, alloc);
		return nullptr;
		//return alloc.new_object<MIPMap>(std::move(image), wrapMode, alloc);
	}
}