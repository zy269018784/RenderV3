#pragma once
#include <cpu_gpu.h>
#include <Util/TaggedPointer.h>

//using namespace Render;
namespace Render
{
	struct SampledWavelengths;
	struct CameraRay;
	struct CameraSample;
	class Ray;
	class PerspectiveCamera;

	class Camera : public TaggedPointer < PerspectiveCamera >
	{
		public:
			using TaggedPointer::TaggedPointer;

			CPU_GPU CameraRay GenerateRay(
				CameraSample sample, SampledWavelengths& lambda) const;
	};
}