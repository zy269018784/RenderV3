#pragma once
#include <cpu_gpu.h>
#include <Type/Types.h>
#include <Type/TypeDeclaration.h>
namespace Render {
    struct BSDFSample;
	class GraniteBxDF {
    public:
        GraniteBxDF() = default;
        CPU_GPU GraniteBxDF(Float reflectivity = 1.0f, Float transmissivity = 1.0f);
        CPU_GPU BSDFSample Sample_f(Vector3f wo, Float uc, Point2f u) const;   
        CPU_GPU BSDFSample f(Vector3f wo, Vector3f wi);
        CPU_GPU Float PDF(Vector3f wo, Vector3f wi)  const { return 1.0; };
    private:
        Float reflectivity;
        Float transmissivity;
	};
}