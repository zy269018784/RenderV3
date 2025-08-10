#pragma once
#include <Math/Math.h>
#include <Math/Transform.h>
#include <Material/Material.h>
#include <Primitive/Primitive.h>
#include <Intersection/Intersection.h>
namespace Render {

    class Primitive;
    // SimplePrimitive Definition
    class TransformedPrimitive {
    public:

        CPU_GPU Bounds3f Bounds() const {
            return renderFromPrimitive(primitive.Bounds());
        }
            
        CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& r, Float tMax = Infinity) const {
            Ray ray = renderFromPrimitive.ApplyInverse(r, &tMax);
            Optional<ShapeIntersection> si = primitive.Intersect(ray, tMax);
            if (!si) 
                return {};
            // Return transformed instance's intersection information
            si->intr = renderFromPrimitive(si->intr);
       
            if (material.Tag() > 0) {
                si->intr.material = material;
            }
            return si;
        }

        std::string ToString() const {
            return {};
        }

    public:
        Primitive primitive;            // 图元
        Material material;              // 材质
        Transform renderFromPrimitive;  // 图元空间 到 渲染空间 
    };
}