#include <Camera/Camera.h>
#include <Camera/PerspectiveCamera.h>
#include <Camera/CameraSample.h>
#include <Camera/CameraRay.h>
#include <Camera/CameraSample.h>
#include <Camera/SampledWavelengths.h>
namespace Render
{
	CameraRay Camera::GenerateRay(CameraSample sample,
		SampledWavelengths& lambda) const {
		auto generate = [&](auto ptr) { return ptr->GenerateRay(sample, lambda); };
		return Dispatch(generate);
	}
}