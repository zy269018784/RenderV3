#include <Primitive/Primitive.h>
#include <Primitive/SimplePrimitive.h>
#include <Primitive/TransformedPrimitive.h>
#include <Primitive/BVHAggregate.h>
#include <Intersection/Intersection.h>
namespace Render {
	Bounds3f Primitive::Bounds() const {
		auto bounds = [&](auto ptr) { return ptr->Bounds(); };
		return Dispatch(bounds);
	}

	Optional<ShapeIntersection> Primitive::Intersect(const Ray& ray, Float tMax) const {
		auto intersect = [&](auto ptr) { return ptr->Intersect(ray, tMax); };
		return Dispatch(intersect);
	}

	std::string Primitive::ToString() const {
		auto tostring = [&](auto ptr) { return ptr->ToString(); };
		return Dispatch(tostring);
	}
}