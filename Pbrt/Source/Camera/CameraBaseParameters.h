#pragma once
#include <Film/Film.h>
#include <Medium/Medium.h>
#include <Camera/CameraTransform.h>
#include <Math/Transform.h>
namespace Render {
    struct CameraBaseParameters {
        Transform cameraFromWorld;
        Transform worldFromCamera;
        Float shutterOpen = 0, shutterClose = 1;
        Film film;
        Medium medium;
        CameraBaseParameters() = default;
        CameraBaseParameters(const Transform &cameraFromWorld, const Transform &worldFromCamerm, Film film, Medium medium);
    };
}