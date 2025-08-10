#include <Light/PointLight.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Camera/SampledWavelengths.h>
#include <Math/Math.h>
#include <Type/Types.h>
namespace Render {

    PointLight::PointLight(Transform worldFromLight, RGB L, Float power, Float range)
        : LightBase(worldFromLight), L(L), power(power), range(range) {
        this->L.r = Clamp(L.r / 255.f, 0.f, 1.f);
        this->L.g = Clamp(L.g / 255.f, 0.f, 1.f);
        this->L.b = Clamp(L.b / 255.f, 0.f, 1.f);
    }

    PointLight* PointLight::Create(const Transform& worldFromLight,
        RGB L, Float power, Float range, Point3f from, Allocator alloc) {
        Transform tf = Translate(Vector3f(from.x, from.y, from.z));
        Transform finalRenderFromLight(worldFromLight * tf);
        return alloc.new_object<PointLight>(finalRenderFromLight, L, power, range);
    }

    CPU_GPU Float PointLight::Power(Float d) const
    {
        if (d < range)
            return power;
        return exp(range - d) * power;
    }

    CPU_GPU LightLiSample PointLight::SampleLi(
        LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
        bool allowIncompletePDF) const {
        // light space 转 world space, 获取光源的世界坐标。 
        Point3f p = worldFromLight(Point3f(0, 0, 0));
        // 交点到光源的方向向量
        Vector3f wi = Normalize(p - ctx.p());
        // 交点到光源的距离
        Float d = Length(p - ctx.p());
        // 计算光照强度
        RGB  Li = L * Power(d);

        int pw = Power(d);
        return LightLiSample(Li, RGB(pw, pw, pw), wi, 1, SurfaceInteraction(p));
    }
}