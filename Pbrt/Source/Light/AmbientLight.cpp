#include <Light/AmbientLight.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Camera/SampledWavelengths.h>
#include <Math/Math.h>
#include <Type/Types.h>
namespace Render {
   
    AmbientLight::AmbientLight(Transform worldFromLight, RGB L, Float power)
        :   LightBase(worldFromLight), L(L), power(power)
    {
        this->L.r = Clamp(L.r / 255.f, 0.f, 1.f);
        this->L.g = Clamp(L.g / 255.f, 0.f, 1.f);
        this->L.b = Clamp(L.b / 255.f, 0.f, 1.f);
    }
     
    AmbientLight* AmbientLight::Create(const Transform& worldFromLight, 
        RGB L, Float power, Allocator alloc) {


        return alloc.new_object<AmbientLight>(worldFromLight, L, power);
    }


    CPU_GPU LightLiSample AmbientLight::SampleLi(
        LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
        bool allowIncompletePDF) const {
        // light space 转 world space, 获取光源的世界坐标。 
        Point3f p = worldFromLight(Point3f(0, 0, 0));
        // 交点到光源的方向向量
        Vector3f wi = Normalize(p - ctx.p());
        // 计算光照强度
        RGB  Li = L * power;

        return LightLiSample(Li, RGB(power, power, power), wi, 1, SurfaceInteraction(p));
       // return LightLiSample(Li, wi, 1, Interaction(p));
    }
}