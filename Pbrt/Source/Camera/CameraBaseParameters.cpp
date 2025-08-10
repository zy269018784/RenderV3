#include <Camera/CameraBaseParameters.h>

namespace Render {
    CameraBaseParameters::CameraBaseParameters(const Transform& cameraFromWorld, 
                                               const Transform& worldFromCamera,
                                               Film film, Medium medium)
        : cameraFromWorld(cameraFromWorld), worldFromCamera(worldFromCamera),
        film(film), medium(medium) {

    }
}