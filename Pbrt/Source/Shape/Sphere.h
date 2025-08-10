#pragma once
#include <Math/Float.h>
#include <Util/Optional.h>
#include <Math/Transform.h>
#include <Primitive/TransformedPrimitive.h>
namespace Render {
    struct ShapeIntersection;
    struct QuadricIntersection;
    class Ray;
    class Sphere {
    public:
        Sphere() = default;
        Sphere(Transform renderFromObject, Transform objectFromRender,
            Float radius, Float zMin, Float zMax, Float phiMax)
            : renderFromObject(renderFromObject),
            objectFromRender(objectFromRender),
            radius(radius),
            zMin(Clamp(std::min(zMin, zMax), -radius, radius)),
            zMax(Clamp(std::max(zMin, zMax), -radius, radius)),
            thetaZMin(std::acos(Clamp(std::min(zMin, zMax) / radius, -1, 1))),
            thetaZMax(std::acos(Clamp(std::max(zMin, zMax) / radius, -1, 1))),
            phiMax(Radians(Clamp(phiMax, 0, 360))) {
            //printf("thetaZMin %f, thetaZMax %f, %f %f\n", thetaZMin, thetaZMax, zMin, zMax);
        }
        void set(Transform renderFromObject, Transform objectFromRender, Float radius) {
            this->renderFromObject = renderFromObject;
            this->objectFromRender = objectFromRender;
            this->radius = radius;
            this->zMin = -radius;
            this->zMax =  radius;
            this->thetaZMin = std::acos(Clamp(std::min(zMin, zMax) / radius, -1, 1));
            this->thetaZMax = std::acos(Clamp(std::max(zMin, zMax) / radius, -1, 1));
            this->phiMax = Radians(360);
        }
        CPU_GPU Bounds3f Bounds() const;
        CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
        CPU_GPU Optional<QuadricIntersection> BasicIntersect(const Ray& r, Float tMax)  const;
        CPU_GPU SurfaceInteraction InteractionFromIntersection(const QuadricIntersection& isect, Vector3f wo, Float time) const;

        //CPU_GPU static Optional<ShapeIntersection> Intersect2(const Ray& ray, Float tMax = Infinity) {
        //
        //}
    public:
        // Sphere Private Members
        Float radius;
        Float zMin, zMax;
        Float thetaZMin, thetaZMax, phiMax;
        Transform renderFromObject, objectFromRender;
        //TransformedPrimitive primitive;
        //bool reverseOrientation, transformSwapsHandedness;
    };
}