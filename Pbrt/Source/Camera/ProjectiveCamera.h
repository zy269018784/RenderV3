#pragma once
#include <Camera/CameraBase.h>
namespace Render {
    class ProjectiveCamera : public CameraBase {
    public:
        // ProjectiveCamera Public Methods
        ProjectiveCamera() = default;
            
        ProjectiveCamera(CameraBaseParameters baseParameters,
            const Transform& screenFromCamera, Bounds2f screenWindow,
            Float lensRadius, Float focalDistance)
            : CameraBase(baseParameters),
            screenFromCamera(screenFromCamera),
            lensRadius(lensRadius),
            focalDistance(focalDistance) {
            // -> NDC
            NDCFromScreen =
                Scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
                    1 / (screenWindow.pMax.y - screenWindow.pMin.y), 1) *
                Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));
            // -> raster
            rasterFromNDC =
                Scale(film.FullResolution().x, -film.FullResolution().y, 1);
            rasterFromScreen = rasterFromNDC * NDCFromScreen;

            // -> screen
            //screenFromCamera = Perspective(fov, 1e-2f, 1000.f);
            screenFromRaster = Inverse(rasterFromScreen);
            screenFromNDC    = Inverse(NDCFromScreen);
            
            // -> NDC
            NDCFromRaster = Inverse(rasterFromNDC);
            NDCFromCamera = NDCFromScreen * screenFromCamera;
            // -> raster   
            rasterFromCamera = rasterFromNDC * NDCFromCamera;
            // -> camera
            cameraFromRaster = Inverse(screenFromCamera) * screenFromRaster;
            cameraFromNDC = cameraFromRaster * rasterFromNDC;
            cameraFromScreen = cameraFromRaster * rasterFromScreen;
        }
    public:
        // ProjectiveCamera Protected Members
        Transform screenFromCamera;     // 已知: Perspective(fov, 1e-2f, 1000.f);
        Transform screenFromRaster;     // Inverse(rasterFromScreen) 
        Transform screenFromNDC;        // Inverse(NDCFromScreen)
        Transform rasterFromScreen;     // rasterFromNDC * NDCFromScreen;
        Transform rasterFromNDC;        // 已知: Scale(film.FullResolution().x, -film.FullResolution().y, 1);
        Transform rasterFromCamera;     // Inverse(cameraFromRaster)
        Transform cameraFromRaster;     // Inverse(screenFromCamera) * screenFromRaster;
        Transform cameraFromNDC;        // cameraFromRaster * rasterFromNDC; 
        Transform cameraFromScreen;     // cameraFromRaster * rasterFromScreen;
        Transform NDCFromScreen;        // 已知:
        Transform NDCFromRaster;        // Inverse(rasterFromNDC) = NDCFromScreen * screenFromRaster
        Transform NDCFromCamera;        // Inverse(cameraFromNDC) = NDCFromScreen * screenFromCamera
        Float lensRadius, focalDistance;
    };

}