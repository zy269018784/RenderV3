#pragma once
#include <cpu_gpu.h>
#include <Math/Transform.h>
#include <Math/AnimatedTransform.h>
#include <Ray/Ray.h>
namespace Render {
    class CameraTransform {
    public:
        // CameraTransform Public Methods
        CameraTransform() = default;
        explicit CameraTransform(const AnimatedTransform& worldFromCamera);

        CPU_GPU
            Point3f RenderFromCamera(Point3f p, Float time) const {
            return renderFromCamera(p, time);
        }
        CPU_GPU
            Point3f CameraFromRender(Point3f p, Float time) const {
            return renderFromCamera.ApplyInverse(p, time);
        }
        CPU_GPU
            Point3f RenderFromWorld(Point3f p) const { return worldFromRender.ApplyInverse(p); }

        CPU_GPU
            Transform RenderFromWorld() const { return Inverse(worldFromRender); }
        CPU_GPU
            Transform CameraFromRender(Float time) const {
            return Inverse(renderFromCamera.Interpolate(time));
        }
        CPU_GPU
            Transform CameraFromWorld(Float time) const {
            return Inverse(worldFromRender * renderFromCamera.Interpolate(time));
        }

        //CPU_GPU
        //    bool CameraFromRenderHasScale() const { return renderFromCamera.HasScale(); }

        CPU_GPU
            Vector3f RenderFromCamera(Vector3f v, Float time) const {
            return renderFromCamera(v, time);
        }

        CPU_GPU
            Normal3f RenderFromCamera(Normal3f n, Float time) const {
            return renderFromCamera(n, time);
        }

        CPU_GPU
            Ray RenderFromCamera(const Ray& r) const { 
            return renderFromCamera(r); }

        //CPU_GPU
        //    RayDifferential RenderFromCamera(const RayDifferential& r) const {
        //    return renderFromCamera(r);
        //}

        CPU_GPU
            Vector3f CameraFromRender(Vector3f v, Float time) const {
            return renderFromCamera.ApplyInverse(v, time);
        }

        CPU_GPU
            Normal3f CameraFromRender(Normal3f v, Float time) const {
            return renderFromCamera.ApplyInverse(v, time);
        }

        CPU_GPU
            const AnimatedTransform& RenderFromCamera() const { return renderFromCamera; }

        CPU_GPU
            const Transform& WorldFromRender() const { return worldFromRender; }

      //  std::string ToString() const;

    //private:
    public:
        AnimatedTransform renderFromCamera;
        Transform worldFromRender;
    };


}