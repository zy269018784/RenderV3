#pragma once
#include <Math/Math.h>
#include <BxDF/BxDF.h>
#include <BxDF/BSDFSample.h>
#include <Util/Frame.h>
#include <BxDF/DiffuseBxDF.h>
namespace Render {
    class BSDF {
    public:
        BSDF() = default;
        CPU_GPU
            BSDF(Normal3f ns, Vector3f dpdus, BxDF bxdf)
            :  bxdf(bxdf), 
            shadingFrame(Frame::FromZ(ns))
           // shadingFrame(Frame::FromXZ(Normalize(dpdus), Vector3f(ns)))
        
        {}
        // 转本地坐标
        CPU_GPU
            Vector3f ToLocal(Vector3f v) const { return shadingFrame.ToLocal(v); }
        // 转世界坐标
        CPU_GPU
            Vector3f FromLocal(Vector3f v) const { return shadingFrame.FromLocal(v); }

        CPU_GPU BSDFSample Sample_f(
            Vector3f woRender, Float u, Point2f u2) const {
            
            //printf("woRender %f %f %f\n", woRender.x, woRender.y, woRender.z);
            Vector3f wo = ToLocal(woRender);
            //printf("wo %f %f %f\n", wo.x, wo.y, wo.z);

            if (wo.z == 0)
                return {};

            BSDFSample bs = bxdf.Sample_f(wo, u, u2);
            //printf("bs.wi %f %f %f, %f\n", bs.wi.x, bs.wi.y, bs.wi.z, u);
            
            bs.wi = FromLocal(bs.wi);
            //printf("BBBB bs.wi %f %f %f\n", bs.wi.x, bs.wi.y, bs.wi.z);
            return bs;
        }

        CPU_GPU BSDFSample f(Vector3f woRender, Vector3f wiRender) {
            Vector3f wi = ToLocal(wiRender), wo = ToLocal(woRender);
            if (wo.z == 0)
                return {};

            return bxdf.f(wo, wi);
        }

        CPU_GPU Float PDF(Vector3f woRender, Vector3f wiRender) const {
            Vector3f wo = ToLocal(woRender), wi = ToLocal(wiRender);
            if (wo.z == 0)
                return 0;
            return bxdf.PDF(wo, wi);
        }

    public:
        BxDF bxdf;
        Frame shadingFrame;
    };
}