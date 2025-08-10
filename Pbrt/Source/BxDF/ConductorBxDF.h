#pragma once
#include <cpu_gpu.h>
#include <Type/Types.h>
#include <Type/TypeDeclaration.h>
#include <CookTorrance/TrowbridgeReitzDistribution.h>
namespace Render {
    struct BSDFSample;

	class ConductorBxDF {
    public:
        ConductorBxDF() = default;
        CPU_GPU
            ConductorBxDF(const TrowbridgeReitzDistribution& mfDistrib)
            : mfDistrib(mfDistrib) {}
        CPU_GPU
            BSDFSample Sample_f(Vector3f wo, Float uc, Point2f u) const; 
        CPU_GPU BSDFSample f(Vector3f wo, Vector3f wi);
        CPU_GPU Float PDF(Vector3f wo, Vector3f wi) const;
    private:
        // ConductorBxDF Private Members
        TrowbridgeReitzDistribution mfDistrib;
	};
}