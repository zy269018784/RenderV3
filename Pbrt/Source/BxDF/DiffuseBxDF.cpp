#include <BxDF/DiffuseBxDF.h>
#include <BxDF/BSDFSample.h>
#include <Math/Sampling.h>

namespace Render {

    //CPU_GPU DiffuseBxDF::DiffuseBxDF(const RGB& R) : R(R) {
    //    
    //}

    CPU_GPU
        BSDFSample DiffuseBxDF::Sample_f(
            Vector3f wo, Float uc, Point2f u) const {
        // cos半球采样
        Vector3f wi = SampleCosineHemisphere(u);

        if (wo.z < 0)
            wi.z *= -1;
        Float pdf = CosineHemispherePDF(AbsCosTheta(wi));

        return BSDFSample(1.f * InvPi,  wi, pdf);
    }

    CPU_GPU BSDFSample DiffuseBxDF::f(Vector3f wo, Vector3f wi)
    {
        if (!SameHemisphere(wo, wi))
            return BSDFSample();
        return BSDFSample(1.0 * InvPi, wo, 0);
    }
}