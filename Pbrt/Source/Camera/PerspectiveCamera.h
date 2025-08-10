#pragma once
#include <cpu_gpu.h>
#include <Camera/ProjectiveCamera.h>


using namespace Render;
namespace Render {

	struct SampledWavelengths;
	struct CameraRay;
	struct CameraSample;
	class Ray;

	class PerspectiveCamera : public ProjectiveCamera
	{
		public:
			CPU_GPU PerspectiveCamera() =default;
			CPU_GPU PerspectiveCamera(CameraBaseParameters baseParameters, Float fov,
				Bounds2f screenWindow, Float lensRadius, Float focalDist)
				: ProjectiveCamera(baseParameters, Perspective(fov, 1e-2f, 1000.f), screenWindow,
					lensRadius, focalDist) {
				cosTotalWidth = 0;
				A = 0;
				dxCamera = {};
				dyCamera = {};
			}
			CPU_GPU
				CameraRay GenerateRay(CameraSample sample,
					SampledWavelengths& lambda) const;

	private:
		// PerspectiveCamera Private Members
		Vector3f dxCamera, dyCamera;
		Float cosTotalWidth;
		Float A;
	};
}