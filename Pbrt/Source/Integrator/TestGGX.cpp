#include <Integrator/TestGGX.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
#include <Camera/CameraSample.h>
#include <Camera/CameraRay.h>
#include <Ray/Ray.h>
#include <Intersection/Intersection.h>
#include <Intersection/Interaction.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Scene/Scene.h>
#include <CookTorrance/TrowbridgeReitzDistribution.h>
#include <Math/Scattering.h>
// 渲染体积云
// https://www.shadertoy.com/view/ltlSWB 
namespace Render {
    TestGGX::TestGGX(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    Optional<ShapeIntersection> TestGGX::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    CPU_GPU Float Intersect_Ray_Sphere(
        Vector3f rp, Vector3f rd,
        Vector3f sp, Float sr2,
        Vector2f t)
    {
        rp -= sp;

        float a = Dot(rd, rd);
        float b = 2.0 * Dot(rp, rd);
        float c = Dot(rp, rp) - sr2;

        float D = b * b - 4.0 * a * c;

        if (D < 0.0) return 0.0;

        float sqrtD = std::sqrt(D);
        t = (-b + Vector2f(-sqrtD, sqrtD)) / a * 0.5f;

        // if(start == inside) ...
        if (c < 0.0) {
            float tmp = t.x;
            t.x = t.y;
            t.y = tmp;
        }

        // t.x > 0.0 || start == inside ? infront : behind
        return t.x > 0.0 || c < 0.0 ? 1.0 : -1.0;
    }

    CPU_GPU float SchlickFresnel(float cosTheta, float Rs) {
        return Rs + pow(1.0 - cosTheta, 5.) * (1. - Rs);
    }



    RGB TestGGX::Li(const Ray& ray, Float u, Float tMax) const {

        RGB sky(0, 0, 0);

        return sky;
    }

    RGB TestGGX::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        if (1 == scene->SampleCount) {
            acc_color[0] = 0.f;
            acc_color[1] = 0.f;
            acc_color[2] = 0.f;
        }

        Point2f ps = scene->sampler.Get2D();

        CameraSample cs;
        cs.pFilm.x = col + ps.x;
        cs.pFilm.y = row + ps.y;

        SampledWavelengths w;

        CameraRay r = scene->camera.GenerateRay(cs, w);
        TrowbridgeReitzDistribution mfDistrib(0.5f, 0.5f);
        RGB color(0, 0, 0);
        auto intr = Intersect(r.ray);
        if (intr) {
            color = intr->intr.material.Evaluate(intr->intr, w);
            // 入射光
            Vector3f wo = Normalize(-r.ray.d);
            //Point2f u = scene->sampler.Get2D();
            //Point2f u(std::sin(col * 1.f) * 43758.5453123f, std::cos(row * 1.f + 5412.8784f) * 43758.5453123f);
            Point2f u(std::sin(scene->SampleCount * 31233.413f) * 43758.5453123f, std::cos(scene->SampleCount * 31233.413f + col) * 43758.5453123f);
            u.x = Clamp(u.x, 0.f, 1.f);
            u.y = Clamp(u.y, 0.f, 1.f);
            // 微平面法线
            Vector3f wm = mfDistrib.Sample_wm(wo, u);
            // 出射光
            //auto bsdf = intr->intr.GetBSDF();
            //auto sample = bsdf.Sample_f(wo, 0, {0, 0});
            //Vector3f wi = sample.wi;
            // 出射光
            Vector3f wi = Reflect(wo, wm);
            //if (!SameHemisphere(wo, wi))
            //    return {};

            Vector3f wh = (wi + wo);
            wh = Normalize(wh);

            Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
            //if (cosTheta_i == 0 || cosTheta_o == 0)
            //    return {};

            Float D = mfDistrib.D(wm);

            Float G = mfDistrib.G(wo, wi);

            //Float F = FrDielectric(Dot(wo, wm), 0.1f);

            Float F = FrDielectric(Dot(wo, wh), 0.1f);

           // Float F = SchlickFresnel(Dot(wo, wh), 0.1f);

            Float f = D * F * G / (4.f * cosTheta_o * cosTheta_i);
            //f =  F / (4.f * cosTheta_o * cosTheta_i);

            color = (1.f - f) * RGB(1, 1, 1);
        }

        //pixel_color[0] = color.r;
        //pixel_color[1] = color.g;
        //pixel_color[2] = color.b;

        acc_color[0] += color.r;
        acc_color[1] += color.g;
        acc_color[2] += color.b;
        
        pixel_color[0] = acc_color[0] / scene->SampleCount * 255;
        pixel_color[1] = acc_color[1] / scene->SampleCount * 255;
        pixel_color[2] = acc_color[2] / scene->SampleCount * 255;
        return {};
    }

}