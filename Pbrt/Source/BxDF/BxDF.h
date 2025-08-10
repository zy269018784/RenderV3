#pragma once
#include <cpu_gpu.h>
#include <Math/Math.h>
#include <Util/TaggedPointer.h>
namespace Render {
	class DiffuseBxDF;
	class DielectricBxDF;
	class ConductorBxDF;
	class GlassBxDF;
	class MirrorBxDF;
	class GraniteBxDF;
	struct BSDFSample;
	class BxDF : public TaggedPointer<DiffuseBxDF, ConductorBxDF, GlassBxDF, DielectricBxDF, MirrorBxDF, GraniteBxDF> {
	public:
		using TaggedPointer::TaggedPointer;

		CPU_GPU  BSDFSample Sample_f(Vector3f wo, Float uc, Point2f u) const;
		CPU_GPU BSDFSample f(Vector3f wo, Vector3f wi);
		CPU_GPU Float PDF(Vector3f wo, Vector3f wi) const;
	};
}

/*
	Sample_f采用局部坐标空间: z为法线方向	
	
*/