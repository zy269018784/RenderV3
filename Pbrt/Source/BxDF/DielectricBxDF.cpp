#include <BxDF/DielectricBxDF.h>
#include <BxDF/BSDFSample.h>
#include <Math/Sampling.h>
#include <Math/Scattering.h>
namespace Render {
    CPU_GPU DielectricBxDF::DielectricBxDF(Float eta) 
        : eta(eta){

    }
    CPU_GPU
        BSDFSample DielectricBxDF::Sample_f(
            Vector3f wo, Float uc, Point2f u) const {
        Float R = FrDielectric(CosTheta(wo), eta);
        Float T = 1.f - R;
        if (uc < 0.5) {
            Vector3f wi(-wo.x, -wo.y, wo.z);
            Float fr = R / AbsCosTheta(wi);
            Float pdf = R / (T + R);
            return BSDFSample(fr, wi, pdf);
        }
        else {
            Vector3f wi;
            Float etap;
            bool valid = Refract(wo, Normal3f(0, 0, 1), eta, &etap, &wi);
            if (!valid)
                return {};

            Float ft = T / AbsCosTheta(wi);
            Float pdf = T / (T + R);

            return BSDFSample(T, wi, pdf);
        }
    }

    CPU_GPU BSDFSample DielectricBxDF::f(Vector3f wo, Vector3f wi)
    {
        return BSDFSample();
    }
}