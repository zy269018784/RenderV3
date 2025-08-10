#pragma once
#include <Util/TaggedPointer.h>
#include <Util/Optional.h>
#include <Math/Math.h>
namespace Render {
    struct ShapeIntersection;
    class Ray;

    class BVHAggregate;
    class SimplePrimitive;
    class TransformedPrimitive;
    class Primitive
        : public TaggedPointer<SimplePrimitive, BVHAggregate, TransformedPrimitive> {
    public:
        using TaggedPointer::TaggedPointer;

        CPU_GPU Bounds3f Bounds() const;

        CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;

        std::string ToString() const;
    };
}