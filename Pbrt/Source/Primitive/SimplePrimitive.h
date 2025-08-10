#pragma once
#include <Math/Math.h>
#include <Shape/Shape.h>
#include <Material/Material.h>
namespace Render {
    struct ShapeIntersection;
    class Ray;

    class SimplePrimitive {
    public:
        SimplePrimitive(Shape shape, Material material);
        CPU_GPU Bounds3f Bounds() const;
        CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
        std::string ToString() const;

    public:
        Shape shape;
        Material material;
    };
}