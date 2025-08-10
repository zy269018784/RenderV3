#pragma once
#include <cpu_gpu.h>
#include <Film/FilmBaseParameters.h>
namespace Render {
	class RGBFilm;
	class FilmBase {
	public:
		FilmBase(FilmBaseParameters p) :
			fullResolution(p.fullResolution),
			filename(p.filename) {}
		CPU_GPU
			Point2i FullResolution() const { return fullResolution; }

		std::string GetFilename() const { return filename; }
	public :
		Point2i fullResolution;
		std::string filename;
	};

}
