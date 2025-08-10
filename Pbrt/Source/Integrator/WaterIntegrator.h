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
	class WaterIntegrator {
	public:
		WaterIntegrator(Primitive aggregate, Scene* scene);
		CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
		CPU_GPU RGB Li(const Ray& ray, Float u, Float tMax = Infinity) const;
		CPU_GPU RGB LiPixel(int row, int col) const;
	private:
		Primitive aggregate;
		Scene* scene;
	};
}