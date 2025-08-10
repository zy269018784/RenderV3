#include <Camera/PerspectiveCamera.h>
#include <Camera/CameraRay.h>
#include <Camera/CameraSample.h>
#include <Camera/SampledWavelengths.h>
#include <Math/Sampling.h>
#if RENDER_IS_GPU_CODE
    #include <cuda.h>
#endif
namespace Render {
    // 
	CameraRay PerspectiveCamera::GenerateRay(CameraSample sample,
		SampledWavelengths& lambda) const {

        Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
        Point3f pCamera = cameraFromRaster(pFilm);

        //Ray ray(Point3f(0, 0, 0), Normalize(Vector3f(pCamera)), SampleTime(sample.time));
        Ray ray(Point3f(0, 0, 0), Normalize(Vector3f(pCamera)));
           
        // Modify ray for depth of field
        if (lensRadius > 0) {
            // Sample point on lens
            Point2f pLens = lensRadius * SampleUniformDiskConcentric(sample.pLens);

            // Compute point on plane of focus
            Float ft = focalDistance / ray.d.z;
            Point3f pFocus = ray(ft);

            // Update ray for effect of lens
            ray.o = Point3f(pLens.x, pLens.y, 0);
            ray.d = Normalize(pFocus - ray.o);
        }
        // 光线转到世界坐标系
        return CameraRay { worldFromCamera(ray)};
	}
}