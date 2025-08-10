#include <Camera/CameraBase.h>

namespace Render {
    CameraBase::CameraBase(CameraBaseParameters p)
        : cameraFromWorld(p.cameraFromWorld), worldFromCamera(p.worldFromCamera),
          shutterOpen(p.shutterOpen), shutterClose(p.shutterClose),
          film(p.film), medium(p.medium) {
    }
}