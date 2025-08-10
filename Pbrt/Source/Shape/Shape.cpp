#include <Shape/Shape.h>
#include <Shape/Triangle.h>
#include <Shape/Sphere.h>
#include <Intersection/Intersection.h>
namespace Render {

	Bounds3f Shape::Bounds() const {
		auto bounds = [&](auto ptr) { return ptr->Bounds(); };
		return Dispatch(bounds);
	}

	Optional<ShapeIntersection> Shape::Intersect(const Ray& ray,
		Float tMax) const {
		auto intr = [&](auto ptr) { return ptr->Intersect(ray, tMax); };
		return Dispatch(intr);
	}
}