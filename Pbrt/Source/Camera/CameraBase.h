#pragma once
#include <cpu_gpu.h>
#include <Camera/CameraBaseParameters.h>
#include <Camera/CameraTransform.h>
#include <Medium/Medium.h>
#include <Film/Film.h>
namespace Render {
    class CameraBase {
    public:
        CameraBase() = default;
        CameraBase(CameraBaseParameters p);

        CPU_GPU
            Film GetFilm() const { return film; }

        CPU_GPU
            Float SampleTime(Float u) const { return Lerp(u, shutterOpen, shutterClose); }

        CPU_GPU
            const Transform& WorldFromCamera() const { return worldFromCamera; }

        CPU_GPU
            const Transform& CameraFromWorld() const { return cameraFromWorld; }

        CPU_GPU
            Ray WorldFromCamera(const Ray& r) const {
                return worldFromCamera(r, 0);
        }

        CPU_GPU
            Vector3f WorldFromCamera(Vector3f v) const {
            return worldFromCamera(v);
        }

        CPU_GPU
            Normal3f WorldFromCamera(Normal3f n) const {
            return worldFromCamera(n);
        }

        CPU_GPU
            Ray CameraFromWorld(const Ray& r) const {
            return cameraFromWorld(r, 0);
        }

        CPU_GPU
            Vector3f CameraFromWorld(Vector3f v) const {
            return cameraFromWorld(v);
        }

        CPU_GPU
            Normal3f CameraFromWorld(Normal3f n) const {
            return cameraFromWorld(n);
        }

    protected:
        Transform cameraFromWorld;
        Transform worldFromCamera;
        Float shutterOpen, shutterClose;
        Film film;
        Medium medium;
    };
}