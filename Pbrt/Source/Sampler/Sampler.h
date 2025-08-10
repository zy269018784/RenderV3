#pragma once
#include <Util/TaggedPointer.h>
#include <Math/Math.h>
namespace Render {
	class HaltonSampler;
	class Sampler : public TaggedPointer <HaltonSampler> {
	public:
		using TaggedPointer::TaggedPointer;
		CPU_GPU void StartPixelSample(Point2i p, int sampleIndex,
			int dimension = 0);

		CPU_GPU Float Get1D();
		CPU_GPU Point2f Get2D();
	};


	//inline Float Sampler::Get1D() {
	//	auto get = [&](auto ptr) { return ptr->Get1D(); };
	//	return Dispatch(get);
	//}
	//
	//inline Point2f Sampler::Get2D() {
	//	auto get = [&](auto ptr) { return ptr->Get2D(); };
	//	return Dispatch(get);
	//}
}