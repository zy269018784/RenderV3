#pragma once
#include <cpu_gpu.h>
#include <Math/Math.h>
#include <Type/Types.h>
#include <Util/TaggedPointer.h>
#include <Film/RGBFilm.h>
#include <string>
namespace Render {
	class RGBFilm;
	class Film : public TaggedPointer <RGBFilm> {
	public:
		using TaggedPointer::TaggedPointer;
		CPU_GPU inline Point2i FullResolution() const;
		std::string GetFilename() const;
		CPU_GPU inline void    Resize(const Point2i& s);
	};

	CPU_GPU
		inline Point2i Film::FullResolution() const {
		auto fr = [&](auto ptr) { return ptr->FullResolution(); };
		return Dispatch(fr);
	}

	

	CPU_GPU inline void Film::Resize(const Point2i& s) {
		auto resize = [&](auto ptr) { return ptr->Resize(s); };
		return Dispatch(resize);
	}

	//std::string Film::GetFilename() const {
	//	auto get = [&](auto ptr) { return ptr->GetFilename(); };
	//	return DispatchCPU(get);
	//}
}
