#include <BxDF/GlassBxDF.h>
#include <BxDF/BSDFSample.h>
#include <Math/Sampling.h>
#include <Math/Scattering.h>
namespace Render {
    CPU_GPU GlassBxDF::GlassBxDF(Float reflectivity, Float transmissivity) 
        : reflectivity(reflectivity), transmissivity(transmissivity){

    }
    CPU_GPU
        BSDFSample GlassBxDF::Sample_f(
            Vector3f wo, Float uc, Point2f u) const {
         if (uc < 0.5) {
             Vector3f wi = Vector3f(-wo.x, -wo.y, wo.z);
             Float pdf = 0.5f;
             return BSDFSample(reflectivity, wi, pdf);
         }
         else {
             Vector3f wi;
             Float pdf = 0.5f;
             Float eta = 1.0f;
             Float etap;
             bool valid = Refract(wo, Normal3f(0, 0, 1), eta, &etap, &wi);
             return BSDFSample(transmissivity, wi, pdf);
         }
        
        //auto eta = 1.5f;
        //Float R = FrDielectric(AbsCosTheta(wo), eta), T = 1 - R;
        //// Compute _R_ and _T_ accounting for scattering between interfaces
        //if (R < 1) {
        //    R += Sqr(T) * R / (1 - Sqr(R));
        //    T = 1 - R;
        //}
        //// Compute probabilities _pr_ and _pt_ for sampling reflection and transmission
        //Float pr = R, pt = T;
        //if (pr == 0 && pt == 0)
        //    return {};
        //if (uc < pr + 0.3) { // 0.3 need more reflection
        //    // Sample perfect specular dielectric BRDF
        //    Vector3f wi(-wo.x, -wo.y, wo.z);
        //    Float fr(R / AbsCosTheta(wi));
        //    return BSDFSample(fr * reflectivity, wi, pr / (pr + pt));

        //}
        //else {
        //    // Sample perfect specular transmission at thin dielectric interface
        //    Vector3f wi = -wo;
        //    Float ft(T / AbsCosTheta(wi));
        //    return BSDFSample(ft * transmissivity, wi, pt / (pr + pt));
        //}
    }
    CPU_GPU BSDFSample GlassBxDF::f(Vector3f wo, Vector3f wi)
    {
        return BSDFSample();
    }

}