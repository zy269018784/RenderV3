#pragma once
#include <Math/Transform.h>
#include <Util/Optional.h>
//#include <Intersection/Interaction.h>
//#include <Intersection/SurfaceInteraction.h>
//#include <Ray.h>
namespace Render {
    class AnimatedTransform {
    public:
        // AnimatedTransform Public Methods
        AnimatedTransform() = default;
        explicit AnimatedTransform(const Transform& t) : AnimatedTransform(t, 0, t, 1) {}
        AnimatedTransform(const Transform& startTransform, Float startTime,
            const Transform& endTransform, Float endTime);

        CPU_GPU
            bool IsAnimated() const { return actuallyAnimated; }

        CPU_GPU
            Ray ApplyInverse(const Ray& r, Float* tMax = nullptr) const;

        CPU_GPU
            Point3f ApplyInverse(Point3f p, Float time) const {
            if (!actuallyAnimated)
                return startTransform.ApplyInverse(p);
            return Interpolate(time).ApplyInverse(p);
        }

        CPU_GPU
            Vector3f ApplyInverse(Vector3f v, Float time) const {
            if (!actuallyAnimated)
                return startTransform.ApplyInverse(v);
            return Interpolate(time).ApplyInverse(v);
        }

        CPU_GPU
            Normal3f operator()(Normal3f n, Float time) const;
        CPU_GPU
            Normal3f ApplyInverse(Normal3f n, Float time) const {
            if (!actuallyAnimated)
                return startTransform.ApplyInverse(n);
            return Interpolate(time).ApplyInverse(n);
        }
        //CPU_GPU
        //    Interaction operator()(const Interaction& it) const;
        //CPU_GPU
        //    Interaction ApplyInverse(const Interaction& it) const;
        //CPU_GPU
        //    SurfaceInteraction operator()(const SurfaceInteraction& it) const;
        //CPU_GPU
        //    SurfaceInteraction ApplyInverse(const SurfaceInteraction& it) const;
        //CPU_GPU
        //    bool HasScale() const { return startTransform.HasScale() || endTransform.HasScale(); }

        //std::string ToString() const;

        CPU_GPU
            Transform Interpolate(Float time) const;

        CPU_GPU
            Ray operator()(const Ray& r, Float* tMax = nullptr) const;
        //CPU_GPU
        //    RayDifferential operator()(const RayDifferential& r, Float* tMax = nullptr) const;
        CPU_GPU
            Point3f operator()(Point3f p, Float time) const;
        CPU_GPU
            Vector3f operator()(Vector3f v, Float time) const;

        CPU_GPU
            Bounds3f MotionBounds(const Bounds3f& b) const;

        CPU_GPU
            Bounds3f BoundPointMotion(Point3f p) const;

        // AnimatedTransform Public Members
        Transform startTransform, endTransform;
        Float startTime = 0, endTime = 1;

    private:
        // AnimatedTransform Private Methods
        CPU_GPU
            static void FindZeros(Float c1, Float c2, Float c3, Float c4, Float c5, Float theta,
                Interval tInterval, Span<Float> zeros, int* nZeros,
                int depth = 8);

        // AnimatedTransform Private Members
        bool actuallyAnimated = false;
        Vector3f T[2];
        Quaternion R[2];
        SquareMatrix<4> S[2];
        bool hasRotation;
        struct DerivativeTerm {
            CPU_GPU
                DerivativeTerm() {}
            CPU_GPU
                DerivativeTerm(Float c, Float x, Float y, Float z) : kc(c), kx(x), ky(y), kz(z) {}
            Float kc, kx, ky, kz;
            CPU_GPU
                Float Eval(Point3f p) const { return kc + kx * p.x + ky * p.y + kz * p.z; }
        };
        DerivativeTerm c1[3], c2[3], c3[3], c4[3], c5[3];
    };

}