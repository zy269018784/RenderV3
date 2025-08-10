#include <Primitive/SimplePrimitive.h>
#include <Intersection/Intersection.h>
namespace Render {
    SimplePrimitive::SimplePrimitive(Shape shape, Material material)
        : shape(shape), material(material) {
    }

    Bounds3f SimplePrimitive::Bounds() const
    {
        return shape.Bounds();
    }

    Optional<ShapeIntersection> SimplePrimitive::Intersect(const Ray& ray, Float tMax) const {
        return shape.Intersect(ray, tMax);
    }

    std::string SimplePrimitive::ToString() const
    {
        return shape.ToString();
    }
}