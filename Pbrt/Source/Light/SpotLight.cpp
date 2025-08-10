#include <Light/SpotLight.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Camera/SampledWavelengths.h>
#include <Math/Math.h>
#include <Type/Types.h>
namespace Render {
   
    SpotLight::SpotLight(Transform worldFromLight, RGB L, Float power, Float range, Float totalWidth, Float falloffStart)
        :   LightBase(worldFromLight), 
            L(L), 
            power(power),
            range(range),
            cosFalloffEnd(std::cos(Radians(totalWidth))),
            cosFalloffStart(std::cos(Radians(falloffStart))) {
        this->L.r = Clamp(L.r / 255.f, 0.f, 1.f);
        this->L.g = Clamp(L.g / 255.f, 0.f, 1.f);
        this->L.b = Clamp(L.b / 255.f, 0.f, 1.f);
    }
     
    SpotLight* SpotLight::Create(const Transform& worldFromLight, 
        RGB L, Float power, Float range, Point3f from, Point3f to, Float coneangle, Float conedelta, Allocator alloc) {

        Transform dirToZ = (Transform)Frame::FromZ(Normalize(to - from));
        Transform t = Translate(Vector3f(from.x, from.y, from.z)) * Inverse(dirToZ);
        Transform finalRenderFromLight = worldFromLight * t;

        return alloc.new_object<SpotLight>(finalRenderFromLight, L, power, range, coneangle, coneangle - conedelta);
    }


    CPU_GPU Float SpotLight::Power(Float d)  const
    {
        return power * SmoothStep(d, range, 0.f);
    }

    CPU_GPU LightLiSample SpotLight::SampleLi(
        LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
        bool allowIncompletePDF) const {
        // light space 转 world space, 获取光源的世界坐标。 
        Point3f p = worldFromLight(Point3f(0, 0, 0));
        // 交点到光源的方向向量
        Vector3f wLight = Normalize(p - ctx.p());
        // 光源到交点的方向向量
       // Vector3f wi = Normalize(worldFromLight.ApplyInverse(-wLight));
        Vector3f wi = wLight;
        // 交点到光源的距离
        Float d = Length(p - ctx.p());
        // 计算光照强度
       // RGB  Li = L * SmoothStep(CosTheta(wLight), cosFalloffEnd, cosFalloffStart) * Power(d);
        RGB  Li = L * Power(d);
    
       //if (fabs(ctx.p().x - 0.1) < 0.0001 && fabs(ctx.p().y) < 0.0001 && fabs(ctx.p().z) < 0.0001) {
       //    //printf("L %f %f %f\n", L.r, L.g, L.b);
       //    printf("Li %f %f %f\n", Li.r, Li.g, Li.b);
       //    printf("ctx.p() %f %f %f\n", ctx.p().x, ctx.p().y, ctx.p().z);
       //    //printf("p %f %f %f\n", p.x, p.y, p.z);
       //    printf("wi %f %f %f\n", wi.x, wi.y, wi.z);
       //    //printf("wLight %f %f %f\n", wLight.x, wLight.y, wLight.z);
       //    //printf("CosTheta(wLight) %f\n", CosTheta(wLight));
       //    //printf("SmoothStep(CosTheta(wLight), cosFalloffEnd, cosFalloffStart) %f\n", SmoothStep(CosTheta(wLight), cosFalloffEnd, cosFalloffStart));
       //    //printf("power %f\n", power);
       //}

        int pw = Power(d);
        return LightLiSample(Li, RGB(pw, pw, pw), wi, 1, SurfaceInteraction(p));

        //return LightLiSample(Li, wi, 1, Interaction(p));
    }
}