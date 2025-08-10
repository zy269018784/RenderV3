#include <Light/DistantLight.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Camera/SampledWavelengths.h>
#include <Math/Math.h>
#include <Type/Types.h>
namespace Render {
   
    DistantLight::DistantLight(Transform worldFromLight, RGB L, Float power, Point3f from, Point3f to)
        :   LightBase(worldFromLight), 
            L(L), 
            power(power),
            from(from),
            to(to) {
        this->L.r = Clamp(L.r / 255.f, 0.f, 1.f);
        this->L.g = Clamp(L.g / 255.f, 0.f, 1.f);
        this->L.b = Clamp(L.b / 255.f, 0.f, 1.f);
    }
     
    DistantLight* DistantLight::Create(const Transform& worldFromLight, 
        RGB L, Float power,  Point3f from, Point3f to, Allocator alloc) {

        Transform dirToZ = (Transform)Frame::FromZ(Normalize(to - from));
        Transform t = Translate(Vector3f(from.x, from.y, from.z)) * Inverse(dirToZ);
        Transform finalRenderFromLight = worldFromLight * t;

        return alloc.new_object<DistantLight>(finalRenderFromLight, L, power, from, to);
    }

    CPU_GPU LightLiSample DistantLight::SampleLi(
        LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
        bool allowIncompletePDF) const {
        // light space 转 world space, 获取光源的世界坐标。 
        Point3f p = ctx.p() + Vector3f(from - to);
        // 光源方向向量
        Vector3f wLight = Normalize(worldFromLight(Vector3f(0, 0, 1)));
        // 交点到光源的方向向量
        Vector3f wi = -wLight;
        //printf("wi %f %f %f, wLight %f %f %f\n", wi.x, wi.y, wi.z, wLight.x, wLight.y, wLight.z);
        // 交点到光源的距离
        Float d = Length(p - ctx.p());
        // 计算光照强度
        RGB  Li = L * power;

        return LightLiSample(Li, RGB(power, power, power), wi, 1, SurfaceInteraction(p));
        //return LightLiSample(Li, wi, 1, Interaction(p));
    }
}