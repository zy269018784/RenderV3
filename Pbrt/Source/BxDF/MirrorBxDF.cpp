#include <BxDF/MirrorBxDF.h>
#include <BxDF/BSDFSample.h>
namespace Render {
    CPU_GPU
        BSDFSample MirrorBxDF::Sample_f(
            Vector3f wo, Float uc, Point2f u) const {
            Vector3f wi = Vector3f(-wo.x, -wo.y, wo.z);
            Float pdf = 1.0f;
            return BSDFSample(1.f, wi, pdf);
    }
    CPU_GPU BSDFSample MirrorBxDF::f(Vector3f wo, Vector3f wi)
    {
        return BSDFSample();
    }
}