#include <Integrator/TestIntegrator.h>
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
#include <Math/Scattering.h>
#include <Math/LowDiscrepancy.h>
#include <Light/HDRLight.h>
namespace Render {
    TestIntegrator::TestIntegrator(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    bool TestIntegrator::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;
        if (intr && intr->tHit < tMax)
            return true;
        return false;
    };

    bool TestIntegrator::Unoccluded(const Interaction& p0, const Interaction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();

        // Float tMax = Length(p1.p() - p0.p());
        // return !IntersectP(ray, tMax);
        return !IntersectP(ray, Infinity);
    };

    bool TestIntegrator::Unoccluded(const SurfaceInteraction& p0, const SurfaceInteraction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();
        // return !IntersectP(ray, tMax); ShadowEpsilon = 0.0001f;
        return !IntersectP(ray, 1.0 - 0.0001f);
    };

   static CPU_GPU inline Float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) {
        Float f = nf * fPdf, g = ng * gPdf;
        if (IsInf(Sqr(f)))
            return 1;
        return Sqr(f) / (Sqr(f) + Sqr(g));
    }

    bool TestIntegrator::IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr && intr->tHit < tMax && intr->intr.material.Tag() == 3) {
            SampledWavelengths lambda;
            //beta = intr->intr.material.GetBSDF(intr->intr, lambda).Sample_f(-ray.d, 1.0f, {}).f;

            beta = intr->intr.material.Evaluate(intr->intr, lambda);
            return true;
        }
        return false;
    };

    bool TestIntegrator::ThroughTransparent(const Interaction& p0, const Interaction& p1, RGB& beta)  const {
        Ray ray;
        ray.o = p0.p();
        ray.d = Normalize(p1.p() - p0.p());
        Float tMax = Length(p1.p() - p0.p());
        bool ret = IntersectTransparent(ray, tMax, beta);
        //beta *= 1.f / (1.f + tMax);
        return ret;
    };

    CPU_GPU RGB TestIntegrator::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray, HaltonSampler sampler) const {
        RGB allLight(0.0, 0.0, 0.0);
        //RGB allLight(0.f, 0.f, 0.f);
         // 光照点的材质
        Material m = intr.material;
        // 光照点的BSDF
        SampledWavelengths lambda;
        BSDF bsdf = m.GetBSDF(intr, lambda);

        auto u = sampler.Get2D();

        if (scene->hdrLight) {
            LightLiSample lightSample = scene->hdrLight->SampleLi(ctx, sampler.Get2D(), {}, true);

            // Evaluate BSDF for light sample and check light visibility
            Vector3f wo = intr.wo, wi = lightSample.wi;
            auto f = bsdf.f(wo, wi).f * AbsDot(wi, intr.shading.n);

            if (!f || !Unoccluded(intr, lightSample.pLight)) {
                return {};
            } 
            Ray shadowRay(intr.p(), wi);
            Optional<ShapeIntersection> intr = Intersect(shadowRay);
            if (intr) {
                return {};
            }

            // Return light's contribution to reflected radiance
           /* Float p_l = lightSample.p * lightSample.pdf;*/
            Float p_l = 1.0 * lightSample.pdf;

            Float p_b = bsdf.PDF(wo, wi);
            Float w_l = PowerHeuristic(1, p_l, 1, p_b);
            //return lightSample.L * f;


          //  return lightSample.L;
            return w_l * lightSample.L * f / p_l * lightSample.L;
        }

        return allLight;
    }

    Optional<ShapeIntersection> TestIntegrator::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    static CPU_GPU RGB ACESFilm(RGB x)
    {
        float a = 2.51f;
        float b = 0.03f;
        float c = 2.43f;
        float d = 0.59f;
        float e = 0.14f;
        return Clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
    }


    RGB TestIntegrator::Li(const Ray& ray, Float u, Float tMax, HaltonSampler sampler) const {
        RGB beta(1, 1, 1) , L(0, 0, 0);
        int depth = 5;
        Ray currentRay = ray;

        for (int i = 0; i < depth; i++) {
            Optional<ShapeIntersection> intr = Intersect(currentRay, tMax);
            if (intr) {
                auto tag = intr->intr.material.Tag();
                if (tag > 0) {
                    SampledWavelengths lambda;
                    BSDF bsdf = intr->intr.material.GetBSDF(intr->intr, lambda);
                    // 解析法线贴图
                    bsdf = intr->intr.GetBSDF();
                    // 漫反射材质
                    if (1 == tag) {
                        Point2f u2 = sampler.Get2D();
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, u2);
                        auto albedo = intr->intr.material.Evaluate(intr->intr, {});
                        if (albedo.a <= 0) { // completely transparent
                            currentRay.o = intr->intr.p();
                        }
                        else {
                            beta *= RGB(albedo);
                            LightSampleContext ctx(intr->intr);
                            auto Ld = SampleLd(ctx, intr->intr, currentRay, sampler);
                            L += (beta * Ld);
                            auto wi = sample.wi;
                            Vector3f n = intr->intr.shading.n;
                            beta *= sample.f * AbsDot(wi, n) / sample.pdf;
                            currentRay.d = Normalize(sample.wi);
                            currentRay.o = intr->intr.p();
                        }
                    }
                    // 导体
                    else if (2 == tag) {
                        Point2f u2 = sampler.Get2D();
                        //Point2f u2(std::sin(scene->SampleCount * 31233.413f) * 43758.5453123f, std::cos(scene->SampleCount * 43255.f) * 43758.5453123f);
                        
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, u2);
                        beta *= intr->intr.material.Evaluate(intr->intr, {});
                        LightSampleContext ctx(intr->intr);
                        auto Ld = SampleLd(ctx, intr->intr, currentRay, sampler);
                        L += (beta * Ld);
                        auto wi = sample.wi;
                        Vector3f n = intr->intr.shading.n;
                        if(sample.pdf > 0)
                            beta *= sample.f * AbsDot(wi, n) / sample.pdf;

                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
                        continue;
                    }
                    // 玻璃
                    else if (3 == tag ) {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, {0, 0});
                        beta *= sample.f * AbsDot(sample.wi, intr->intr.shading.n) / sample.pdf;
                        // 透射
                        if (u > 0.5f)
                        {
                            auto glassColor = intr->intr.material.Evaluate(intr->intr, lambda);
                            beta *= RGB(glassColor);
                        }
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
                    }
                    // 镜面
                    else if (5 == tag) {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, { 0, 0 });
                        beta *= sample.f;
         
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
                    }
                    // 花岗岩
                    else if (6 == tag) {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, sampler.Get2D());
                        beta *= RGB(intr->intr.material.Evaluate(intr->intr, lambda));
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
         
                        auto wi = sample.wi;
                        Vector3f n = intr->intr.shading.n;
                        beta *= sample.f * AbsDot(wi, n) / sample.pdf;

                        //direct light
                        LightSampleContext ctx(intr->intr);
                        auto Ld = SampleLd(ctx, intr->intr, currentRay, sampler);
                        L += (beta * Ld);
                    }
                    else {
                        Point2f u2 = sampler.Get2D();
                        //Point2f u2(std::sin(scene->SampleCount * 31233.413f) * 43758.5453123f, std::cos(scene->SampleCount * 43255.f) * 43758.5453123f);
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, u2);
                        beta *= sample.f;
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
                    }
                }
            }
            else {
                RGB sky(0.4510, 0.6314, 0.8667);

                //Vector3f sunDir = Normalize(Vector3f(422, 100, 266));
                //Float sundot = Clamp(Dot(currentRay.d, sunDir), 0.f, 1.f);
                //sky += RGB(1.f, 1.f, 1.f) * pow(sundot, 256.f) * 0.95f;
                //sky = Clamp(sky, 0, 1);

                if (scene->hdrLight) {
                    auto sample = scene->hdrLight->SampleRay(currentRay);
                    //sky = Clamp(sample.L, 0, 1);
                    sky = sample.L;
                }
                //
                // 曝光
               // sky *= std::exp2(3.f);
               //sky *= std::exp2(1.0f);
                beta *= sky;
                L += beta;
                break;
            }
        }
        //return beta;
        return L;
    }

    static CPU_GPU float to_sRGB(float rgb_color)
    {
        const double a = 0.055;
        if (rgb_color < 0.0031308)
            return 12.92 * rgb_color;
        else
            return (1.0 + a) * std::pow(rgb_color, 1.0 / 2.4) - a;
    }

    RGB TestIntegrator::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

#ifdef OPENGL_RENDER
        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];
#else
        Float* RenderBuffer = (Float*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        Float* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];
#endif

        // pixel_color[0] = 0.5137 * 255; 
        // pixel_color[1] = 0.7373 * 255;
        // pixel_color[2] = 0.9059 * 255;
        // return {};

        if (1 == scene->SampleCount) {
            acc_color[0] = 0.f;
            acc_color[1] = 0.f;
            acc_color[2] = 0.f;
        }

        HaltonSampler sampler = *scene->sampler.Cast<HaltonSampler>();
        auto index = scene->SampleCount;
        auto dim = 6 + (col % 5) * 7;
        sampler.StartPixelSample({ row, col }, index, dim);
        CameraSample cs;
        Point2f p = sampler.Get2D();
        cs.pFilm.x = col + p.x;
        cs.pFilm.y = row + p.y;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        RGB final(0, 0, 0);
        auto tag = FirstMaterialTag(r.ray);
        if (3 == tag)
        {
            //final =  LiGlass(r.ray);
            final = Li(r.ray, sampler.Get1D(), Infinity, sampler);
        }
        else if (6 == tag){
            //final = LiGranite(r.ray);
            final = Li(r.ray, sampler.Get1D(), Infinity, sampler);
        }
        else {
            final = Li(r.ray, sampler.Get1D(), Infinity, sampler);
        }

        //final = scene->hdrLight->SampleRay(r.ray).L;
    //    final = ACESFilm(final);

        acc_color[0] += final.r;
        acc_color[1] += final.g;
        acc_color[2] += final.b;

        auto rr = acc_color[0] / scene->SampleCount;
        auto gg = acc_color[1] / scene->SampleCount;
        auto bb = acc_color[2] / scene->SampleCount;

#ifdef OPENGL_RENDER
#if 0
       RGB beta = ACESFilm({ rr,gg,bb });
       float gamma = 2.2f;
       beta.r = pow(beta.r, 1.0f / gamma);
       beta.g = pow(beta.g, 1.0f / gamma);
       beta.b = pow(beta.b, 1.0f / gamma);
       pixel_color[0] = beta.r * 255;
       pixel_color[1] = beta.g * 255;
       pixel_color[2] = beta.b * 255;
#elif 1
       RGB beta = { rr,gg,bb };
       beta = Clamp(beta, 0, 1);
       pixel_color[0] = to_sRGB(beta.r) * 255;
       pixel_color[1] = to_sRGB(beta.g) * 255;
       pixel_color[2] = to_sRGB(beta.b) * 255;
#else
        RGB beta = ACESFilm({ rr,gg,bb });
        pixel_color[0] = to_sRGB(beta.r) * 255;
        pixel_color[1] = to_sRGB(beta.g) * 255;
        pixel_color[2] = to_sRGB(beta.b) * 255;
#endif

        //RGB beta = RGB( rr,gg,bb );
        //beta = Clamp(beta, 0, 1);
        //pixel_color[0] = beta.r * 255;
        //pixel_color[1] = beta.g * 255;
        //pixel_color[2] = beta.b * 255;
#else
        RGB beta = { rr,gg,bb };
        beta = Clamp(beta, 0, 1);

        pixel_color[0] = beta.r;
        pixel_color[1] = beta.g;
        pixel_color[2] = beta.b;
#endif

        return {};
    }

    RGB TestIntegrator::LiGlass(const Ray& ray) const {
        return Li(ray, 0.f) + Li(ray, 1.f);
    }

    RGB TestIntegrator::LiGranite(const Ray& ray) const {
        return Li(ray, 0.f) + Li(ray, 1.f);
    }

    // tag > 0: 有材质
    unsigned int TestIntegrator::FirstMaterialTag(const Ray& ray, Float tMax) const {
        unsigned int tag = 0;
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            tag = intr->intr.material.Tag();
        }
        return tag;
    }
}