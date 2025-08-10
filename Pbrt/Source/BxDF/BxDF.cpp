#include <BxDF/BxDF.h>
#include <BxDF/DiffuseBxDF.h>
#include <BxDF/DielectricBxDF.h>
#include <BxDF/ConductorBxDF.h>
#include <BxDF/GlassBxDF.h>
#include <BxDF/MirrorBxDF.h>
#include <BxDF/GraniteBxDF.h>
#include <BxDF/BSDFSample.h>
namespace Render {
	CPU_GPU BSDFSample BxDF::Sample_f(Vector3f wo, Float uc, Point2f u) const {
		auto sample_f = [&](auto ptr) -> BSDFSample {
			return ptr->Sample_f(wo, uc, u);
		};
		return Dispatch(sample_f);
	}

	CPU_GPU BSDFSample BxDF::f(Vector3f wo, Vector3f wi)
	{
		auto f = [&](auto ptr) -> BSDFSample {
			return ptr->f(wo, wi);
			};
		return Dispatch(f);
	}

	CPU_GPU Float BxDF::PDF(Vector3f wo, Vector3f wi) const {
		auto pdf = [&](auto ptr) -> Float { return ptr->PDF(wo, wi); };
		return Dispatch(pdf);
	}
}