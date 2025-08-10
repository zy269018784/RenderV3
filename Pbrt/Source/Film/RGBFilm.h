#pragma once
#include <cpu_gpu.h>
#include <Film/FilmBase.h>
namespace Render {
	class RGBFilm : public FilmBase {
	public:
		RGBFilm() = default;
		CPU_GPU RGBFilm(FilmBaseParameters p) :
			FilmBase(p) {}

		CPU_GPU
			void  Resize(const Point2i& s) { fullResolution = s; }

		std::string GetFilename() const {
			return FilmBase::GetFilename();
		}

	};
}