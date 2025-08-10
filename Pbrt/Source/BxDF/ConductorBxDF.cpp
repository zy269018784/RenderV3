#include <BxDF/ConductorBxDF.h>
#include <BxDF/BSDFSample.h>
#include <CookTorrance/NormalDistributionFunction.h>
#include <Math/Scattering.h>
namespace Render {
    CPU_GPU
        BSDFSample ConductorBxDF::Sample_f(Vector3f wo, Float uc, Point2f u) const {

        if (mfDistrib.EffectivelySmooth()) {
            Vector3f wi = Vector3f(-wo.x, -wo.y, wo.z);
            Float f = 1.f;
            Float pdf = 1;
            return BSDFSample(f, wi, pdf);
        }
        
        if (wo.z == 0)
            return {};

        Vector3f wm = mfDistrib.Sample_wm(wo, u);
        Vector3f wi = Reflect(wo, wm);
        //if (wo.z < 0)
        //    wi.z *= -1; //fixme, why need this?
        //if (!SameHemisphere(wo, wi))
        //    return {};

        // Compute PDF of _wi_ for microfacet reflection
        Float pdf = mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));

        Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
        if (cosTheta_i == 0 || cosTheta_o == 0)
            return {};

        //  Float F = 1.f;
        complex<Float> al_eta = { 1.373, 7.618 };
        Float F = FrComplex(AbsDot(wo, wm), al_eta);// FrDielectric(Dot(wo, wm), 0.001f);
        Float D = mfDistrib.D(wm);
        Float G = mfDistrib.G(wo, wi);

        Float f = D * F * G / (4 * cosTheta_i * cosTheta_o);
    //    f = 0.f;
        return BSDFSample(f, wi, pdf);
       // return BSDFSample(R * InvPi, wi, pdf);
    }

    CPU_GPU
        BSDFSample ConductorBxDF::f(Vector3f wo, Vector3f wi) {
  /*      if (!SameHemisphere(wo, wi))
            return {};*/
        if (mfDistrib.EffectivelySmooth())
            return {};
        // Evaluate rough conductor BRDF
        // Compute cosines and $\wm$ for conductor BRDF
        Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
        if (cosTheta_i == 0 || cosTheta_o == 0)
            return {};
        Vector3f wm = wi + wo;
        if (LengthSquared(wm) == 0)
            return {};
        wm = Normalize(wm);
        complex<Float> al_eta = { 1.373, 7.618 };
        float F = FrComplex(AbsDot(wo, wm), al_eta);
        auto f = mfDistrib.D(wm) * F * mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
        return BSDFSample(f, { 0,0,0 }, 0);
    }
    template <typename T>
    CPU_GPU Vector3<T> FaceForward(Vector3<T> v, Normal3<T> n2) {
        return (Dot(v, n2) < 0.f) ? -v : v;
    }
    CPU_GPU Float ConductorBxDF::PDF(Vector3f wo, Vector3f wi) const {
        if (!SameHemisphere(wo, wi))
            return 0;
        //if (mfDistrib.EffectivelySmooth())
        //    return 0;
        // Evaluate sampling PDF of rough conductor BRDF
        Vector3f wm = wo + wi;
        /*CHECK_RARE(1e-5f, LengthSquared(wm) == 0);*/
        //printf("p_b %.12f\n", LengthSquared(wm));
        if (LengthSquared(wm) == 0)
            return 0;
     
        wm = FaceForward(Normalize(wm), Normal3f(0, 0, 1));
        return mfDistrib.D(wo, wm) / (4 * AbsDot(wo, wm));
    }
}