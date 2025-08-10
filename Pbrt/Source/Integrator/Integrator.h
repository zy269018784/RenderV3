#pragma once
#include <Util/TaggedPointer.h>
#include <Util/Optional.h>
#include <Math/Math.h>

namespace Render {
	class TestIntegrator;
	class TestIntegrator2;
	class TestIntegrator3;
	class TestIntegrator4;
	class TestIntegrator5;
	class TestIntegrator6;
	class TestIntegrator7;
	class TestIntegrator8;
	class TestIntegrator9;
	class TestIntegrator10;
	class TestIntegrator11;
	class TestIntegrator12;
	class TestIntegrator13;
	class TestIntegrator14;
	class TestGGX;
	class TestHDR;
	class WaterIntegrator;
	class TestTextureIntegrator;
	class PathIntegrator;
	class NullIntegrator;
	class GlassIntegrator;
	class GlassIntegrator2;
	class Ray;
	struct ShapeIntersection;
	struct RGB;
	class Integrator : public TaggedPointer<TestIntegrator,  TestIntegrator2,  TestIntegrator3,  TestIntegrator4, 
											TestIntegrator5, TestIntegrator6,  TestIntegrator7,  TestIntegrator8,
											TestIntegrator9, TestIntegrator10, TestIntegrator11, TestIntegrator12,
		TestIntegrator13, TestIntegrator14, WaterIntegrator, GlassIntegrator,  GlassIntegrator2,
											TestTextureIntegrator, TestGGX, TestHDR,
		                                    PathIntegrator, NullIntegrator> {
	public:
		using TaggedPointer::TaggedPointer;

		CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;

		CPU_GPU RGB Li(const Ray& ray, Float u, Float tMax = Infinity) const;
		CPU_GPU RGB LiPixel(int row, int col) const;
	};
}