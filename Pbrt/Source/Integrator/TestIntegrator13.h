#pragma once
#include <cpu_gpu.h>
#include <Primitive/Primitive.h>
#include <Math/Math.h>

namespace Render {
	struct ShapeIntersection;
	struct RGB;
	class Ray;
	class Scene;
	class Interaction;
	class LightSampleContext;
	class SurfaceInteraction;
	class TestIntegrator13 {
	public:
		TestIntegrator13(Primitive aggregate, Scene* scene);
		CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
		CPU_GPU RGB Li(const Ray& ray, Float u, Float tMax = Infinity) const;
		CPU_GPU RGB LiPixel(int row, int col) const;

		CPU_GPU Float Cloud(Point3f p) const;
		CPU_GPU Float FBM(Point3f p) const;

	private:
		Primitive aggregate;
		Scene* scene;
	};
}