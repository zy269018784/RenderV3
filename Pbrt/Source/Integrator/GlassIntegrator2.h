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
	class GlassIntegrator2 {
	public:
		GlassIntegrator2(Primitive aggregate, Scene* scene);
		CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
		CPU_GPU RGB Li(const Ray& ray, Float u, Float tMax = Infinity) const;
		CPU_GPU void GlassTransmissionLi(const Ray& ray, RGB& c, RGB& power, Float tMax = Infinity) const;
		CPU_GPU void GlassReflectLi(const Ray& ray, RGB& c, RGB& power, Float tMax = Infinity) const;
		CPU_GPU RGB GlassLi(const Ray& ray1, const Ray& ray2, Float tMax = Infinity) const;
		CPU_GPU RGB LiPixel(int row, int col) const;
		CPU_GPU bool IntersetGlass(const Ray& ray, Float tMax = Infinity) const;


		CPU_GPU RGB SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& si, const Ray& ray, RGB& color, RGB& power) const;

		CPU_GPU bool IntersectP(const Ray& ray, Float tMax = Infinity) const;

		CPU_GPU bool Unoccluded(const Interaction& p0, const Interaction& p1)  const;

		CPU_GPU bool IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const;

		CPU_GPU bool ThroughTransparent(const Interaction& p0, const Interaction& p1, RGB& beta)  const;


	private:
		Primitive aggregate;
		Scene* scene;
	};
}