#include <BxDF/GraniteBxDF.h>
#include <BxDF/BSDFSample.h>
#include <Math/Sampling.h>
#include <Math/Scattering.h>
namespace Render {
    CPU_GPU GraniteBxDF::GraniteBxDF(Float reflectivity, Float transmissivity) 
        : reflectivity(reflectivity), transmissivity(transmissivity){
    }
    CPU_GPU
        BSDFSample GraniteBxDF::Sample_f(
            Vector3f wo, Float uc, Point2f u) const {
        if (uc < reflectivity) {
            Vector3f wi = Vector3f(-wo.x, -wo.y, wo.z);
            //Vector3f u2 = SampleCosineHemisphere(u) * Vector3f(1.0 - reflectivity);
            Float pdf = 1.0f;
            return BSDFSample(reflectivity, Normalize(wi), pdf);
        }
        else {
            Vector3f wi = SampleCosineHemisphere(u);
            if (wo.z < 0)
                wi.z *= -1;
            Float pdf = CosineHemispherePDF(AbsCosTheta(wi));
            return BSDFSample(1.f * InvPi, wi, pdf);
        }
    }

    CPU_GPU BSDFSample GraniteBxDF::f(Vector3f wo, Vector3f wi)
    {
        //return BSDFSample();
        if (!SameHemisphere(wo, wi))
            return BSDFSample();
        return BSDFSample(1.0 * InvPi, wo, 0);
    }
}