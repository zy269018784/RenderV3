#pragma once
#include <Math/Math.h>

namespace Render {
    // SimplePrimitive Definition
    class KdTreeAggregate {
    public:
        // SimplePrimitive Public Methods
        Bounds3f Bounds() const;
       //pstd::optional<ShapeIntersection> Intersect(const Ray& r, Float tMax) const;
       //bool IntersectP(const Ray& r, Float tMax) const;
       //SimplePrimitive(Shape shape, Material material);

    private:
        // SimplePrimitive Private Members
        //Shape shape;
        //Material material;
    };
}