#pragma once
#include <Util/TaggedPointer.h>
#include <Util/Optional.h>
#include <Math/Math.h>

namespace Render {
    struct ShapeIntersection;
    class Ray;
    class Sphere;
    class Triangle;

    enum ShapeType {
        TriangleType = 0,
        SphereType = 2
    };

    class Shape
        : public TaggedPointer<Sphere, Triangle> {
    public:
        using TaggedPointer::TaggedPointer;

        CPU_GPU
            Bounds3f Bounds() const;

        CPU_GPU 
            Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
    };
}