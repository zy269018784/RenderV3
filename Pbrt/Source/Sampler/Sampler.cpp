#include <Sampler/Sampler.h>
#include <Sampler/HaltonSampler.h>
namespace Render {
	void Sampler::StartPixelSample(Point2i p, int sampleIndex, int dimension) {
		auto start = [&](auto ptr) {
			return ptr->StartPixelSample(p, sampleIndex, dimension);
		};
		return Dispatch(start);
	}

	Float Sampler::Get1D() {
		auto get = [&](auto ptr) { return ptr->Get1D(); };
		return Dispatch(get);
	}

	Point2f Sampler::Get2D() {
		auto get = [&](auto ptr) { return ptr->Get2D(); };
		return Dispatch(get);
	}
}