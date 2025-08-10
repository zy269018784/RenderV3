#include <Integrator/GlassIntegrator2.h>
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

namespace Render {
    GlassIntegrator2::GlassIntegrator2(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    bool GlassIntegrator2::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;
        if (intr && intr->tHit < tMax)
            return true;
        return false;
    };

    bool GlassIntegrator2::Unoccluded(const Interaction& p0, const Interaction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();

        // Float tMax = Length(p1.p() - p0.p());
        // return !IntersectP(ray, tMax);
        return !IntersectP(ray, Infinity);
    };

    bool GlassIntegrator2::IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr && intr->tHit < tMax && intr->intr.material.Tag() == 3) {
            SampledWavelengths lambda;
            //beta = intr->intr.material.GetBSDF(intr->intr, lambda).Sample_f(-ray.d, 1.0f, {}).f;

            beta = intr->intr.material.Evaluate(intr->intr, lambda);
            return true;
        }
        return false;
    };

    bool GlassIntegrator2::ThroughTransparent(const Interaction& p0, const Interaction& p1, RGB& beta)  const {
        Ray ray;
        ray.o = p0.p();
        ray.d = Normalize(p1.p() - p0.p());
        Float tMax = Length(p1.p() - p0.p());
        bool ret = IntersectTransparent(ray, tMax, beta);
        //beta *= 1.f / (1.f + tMax);
        return ret;
    };

    CPU_GPU RGB GlassIntegrator2::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray, RGB& color, RGB& power) const {
        RGB allLight(0.4e5, 0.4e5, 0.4e5);
        //RGB allLight(0.f, 0.f, 0.f);

        SampledWavelengths lambda;
         // 光照点的材质
        Material m = intr.material;
        // 光照点的BSDF
        BSDF bsdf = m.GetBSDF(intr, lambda);
        // 光照点的局部坐标
        Vector3f wo = bsdf.ToLocal(-ray.d);
        // 遍历光源
        for (int i = 0; i < scene->nLight; i++) {
            LightLiSample lightSample = scene->light[i].SampleLi(ctx, {}, {});
            // 光照点的局部坐标
            Vector3f wi = bsdf.ToLocal(lightSample.wi);
            if (!SameHemisphere(wo, wi)) {
                continue;
            }

            Ray currentRay;
            currentRay.d = Normalize(lightSample.pLight.p() - ctx.p());
            currentRay.o = ctx.p();
            RGB beta(1, 1, 1);
            int depth = 5;
            while (1) {
                if (depth == 0) {
                    break;
                }
                depth--;
                Optional<ShapeIntersection> si = Intersect(currentRay);
                if (si) {
                    auto tag = si->intr.material.Tag();
                    // 透过玻璃
                    if (3 == tag) {
                        currentRay.o = si->intr.p();
                        //BSDF bsdf = si->intr.material.GetBSDF(si->intr, lambda);
                        //BSDFSample sample = bsdf.Sample_f(-currentRay.d, 1.0f, { 0, 0 });
                        //beta *= sample.f;
                        beta *= si->intr.material.Evaluate(si->intr, lambda);                     
                    }
                    // 不透明物体
                    else if (tag > 0) {
                        beta *= 0;
                        break;
                    }
                }
                else {
                    beta *= lightSample.L;
                    break;
                }
            }
            allLight += beta;
        }

        //allLight.r = Clamp(allLight.r, 0.f, 1e5f);
        //allLight.g = Clamp(allLight.g, 0.f, 1e5f);
        //allLight.b = Clamp(allLight.b, 0.f, 1e5f);
        //
        //allLight.r /= 1e5;
        //allLight.g /= 1e5;
        //allLight.b /= 1e5;

        return allLight;
    }

    Optional<ShapeIntersection> GlassIntegrator2::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }


#if 1
    RGB GlassIntegrator2::Li(const Ray& ray, Float u, Float tMax) const {
        RGB beta(1, 1, 1);
        int depth = 5;
        Ray currentRay = ray;
        for (int i = 0; i < depth; i++) {
            Optional<ShapeIntersection> intr = Intersect(currentRay, tMax);
            if (intr) {
                auto tag = intr->intr.material.Tag();
                if (tag > 0) {
                    SampledWavelengths lambda;
                    BSDF bsdf = intr->intr.material.GetBSDF(intr->intr, lambda);
                    if (1 == tag) {
                        beta *= intr->intr.material.Evaluate(intr->intr, lambda);
                        // 直接光照
                        RGB power, color;
                        LightSampleContext ctx;
                        ctx.pi = intr->intr.pi;
                        RGB allLight = SampleLd(ctx, intr->intr, ray, color, power);
                        beta *= allLight;
                        break;
                    }
                    else if (3 == tag) {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, { 0, 0 });
                        beta *= sample.f;
                        //beta *= RGB(0.f, 1.f, 0.f);
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
                    }
                    else {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, { 0, 0 });
                        beta *= sample.f;
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
                    }
                }
            }
            else {
                RGB sky(0.4431f, 0.6431f, 0.9176);
                //sky *= Clamp(ray.d.y * 1.5f, 0.f, 1.0f);
                beta *= sky;
                break;
            }
        }
        return beta;
    }
#else
    RGB GlassIntegrator2::Li(const Ray& ray, Float u, Float tMax) const {
        RGB beta(0.f, 0.f, 0.f);
        Ray r = ray;

        return beta;
    }
#endif

    CPU_GPU void GlassIntegrator2::GlassTransmissionLi(const Ray& ray, RGB &color, RGB &power, Float tMax) const {
        RGB beta(1.f, 1.f, 1.f);
        Ray r = ray;
        int depth = 5;
        power = RGB(1.f, 1.f, 1.f) * 1e5f;

        for (int i = 0; i < depth; i++) {
            SampledWavelengths lambda;
            Optional<ShapeIntersection> intr = Intersect(r, tMax);
            if (intr) {
                int tag = intr->intr.material.Tag();
                RGB c = intr->intr.material.Evaluate(intr->intr, lambda);
                beta *= c;
                if (1 == tag) {
                    // 直接光照
                    LightSampleContext ctx;
                    ctx.pi = intr->intr.pi;
                    RGB allLight = SampleLd(ctx, intr->intr, ray, color, power);
                    //allLight = Clamp(allLight, 0.f, 1e5f);
                    //allLight /= 1e5f;
                    
                    beta *= allLight;
                    break; 
                }
                else if (3 == tag) {
                    r.o = intr->intr.p();
                }
            }
        }
        color = beta;
    }

    CPU_GPU void GlassIntegrator2::GlassReflectLi(const Ray& ray, RGB& color, RGB& power, Float tMax) const {
        power = RGB(1.f, 1.f, 1.f) * 0.5e5f;
        SampledWavelengths lambda;
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            power = RGB(1.f, 1.f, 1.f) * 1e5f;
            color = intr->intr.material.Evaluate(intr->intr, lambda) * power;
        }
        else {
            // 直接光照
            //LightSampleContext ctx;
            //ctx.pi = intr->intr.pi;
            //RGB allLight = SampleLd(ctx, intr->intr, ray, color, power);
            //color = allLight;

            power = RGB(1.f, 1.f, 1.f) * 1e5f;
            color = RGB(0.4431f, 0.6431f, 0.9176) * power;    
        }
    }
       
    //  power1 s1 * s2 * c1 * c2 
    CPU_GPU RGB  GlassIntegrator2::GlassLi(const Ray& ray1, const Ray& ray2, Float tMax) const {
        RGB c[3];
        RGB power[2];

        GlassTransmissionLi(ray1, c[0], power[0], tMax);



        GlassReflectLi(ray2, c[1], power[1], tMax);

        //power[0] = Clamp(power[0], 0.f, 1e5f);
        //power[0] /= 1e5;
        //
        //power[1] = Clamp(power[1], 0.f, 1e5f);
        //power[1] /= 1e5;
        //
        //RGB s0 = power[0] / (power[0] + power[1]);
        //RGB s1 = power[1] / (power[0] + power[1]);


        c[2] = c[0] + c[1];
        //c[2] *= 0.5f;
        c[2] = Clamp(c[2], 0.f, 1e5f);
        c[2] /= 1e5f;
        return c[2];

        //return s1 * c[1];
        //return s0 * c[0] ;
        //return s0 * c[0] + s1 * c[1];
    }

    RGB GlassIntegrator2::LiPixel(int row, int col) const {

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

        auto p = scene->sampler.Get2D();

        CameraSample cs;
        cs.pFilm.x = col + p.x;
        cs.pFilm.y = row + p.y;

        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);
            
        RGB final(0, 0, 0);
        Optional<ShapeIntersection> intr = Intersect(r.ray);
        if (intr) {
            if (intr->intr.material.Tag() == 3) {
                Ray ray;
                ray.o = intr->intr.p();
                ray.d = Reflect(-r.ray.d, intr->intr.shading.n);
                final += GlassLi(r.ray, ray);
            }
            else {

                final = intr->intr.material.Evaluate(intr->intr, SampledWavelengths());

                RGB power;
                RGB color;

                // 直接光照
                LightSampleContext ctx;
                ctx.pi = intr->intr.pi;
                RGB allLight = SampleLd(ctx, intr->intr, r.ray, color, power);

                allLight = Clamp(allLight, 0.f, 1e5f);
                allLight /= 1e5f;
                
                final *= allLight;        
            }
        }

        acc_color[0] += final.r;
        acc_color[1] += final.g;
        acc_color[2] += final.b;

        pixel_color[0] = acc_color[0] / scene->SampleCount * 255;
        pixel_color[1] = acc_color[1] / scene->SampleCount * 255;
        pixel_color[2] = acc_color[2] / scene->SampleCount * 255;

        return {};
    }

    bool GlassIntegrator2::IntersetGlass(const Ray& ray, Float tMax) const {
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            auto tag = intr->intr.material.Tag();
            if (3 == tag)
                return true;
        }
        return false;
    }
}