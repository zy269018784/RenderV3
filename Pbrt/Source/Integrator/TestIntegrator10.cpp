#include <Integrator/TestIntegrator10.h>
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
#include <Light/HDRLight.h>
#include <Math/LowDiscrepancy.h>
namespace Render {
    TestIntegrator10::TestIntegrator10(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    bool TestIntegrator10::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;
        if (intr && intr->tHit < tMax)
            return true;
        return false;
    };

    bool TestIntegrator10::Unoccluded(const Interaction& p0, const Interaction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();

        // Float tMax = Length(p1.p() - p0.p());
        // return !IntersectP(ray, tMax);
        return !IntersectP(ray, Infinity);
    };

    bool TestIntegrator10::IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr && intr->tHit < tMax && intr->intr.material.Tag() == 3) {
            SampledWavelengths lambda;
            //beta = intr->intr.material.GetBSDF(intr->intr, lambda).Sample_f(-ray.d, 1.0f, {}).f;

            beta = intr->intr.material.Evaluate(intr->intr, lambda);
            return true;
        }
        return false;
    };

    bool TestIntegrator10::ThroughTransparent(const Interaction& p0, const Interaction& p1, RGB& beta)  const {
        Ray ray;
        ray.o = p0.p();
        ray.d = Normalize(p1.p() - p0.p());
        Float tMax = Length(p1.p() - p0.p());
        bool ret = IntersectTransparent(ray, tMax, beta);
        //beta *= 1.f / (1.f + tMax);
        return ret;
    };

    CPU_GPU RGB TestIntegrator10::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray) const {
        RGB allLight(0.4f, 0.4f, 0.4f);
        //RGB allLight(0.7f, 0.7f, 0.7f);
        // RGB allLight(7e4f, 7e4f, 7e4f);
        //RGB allLight(0.f, 0.f, 0.f);
        SampledWavelengths lambda;
        for (int i = 0; i < scene->nLight; i++) {
            LightLiSample lightSample = scene->light[i].SampleLi(ctx, {}, {});

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
                    if (3 == tag) {
                        currentRay.o = si->intr.p();
                        BSDF bsdf = si->intr.material.GetBSDF(si->intr, lambda);
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, 1.0f, { 0, 0 });
                        beta *= sample.f;
                    }
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
#if 1
        allLight.r /= (1 + allLight.r);
        allLight.g /= (1 + allLight.g);
        allLight.b /= (1 + allLight.b);

#else
        allLight.r = Clamp(allLight.r, 0.f, 1e5f);
        allLight.g = Clamp(allLight.g, 0.f, 1e5f);
        allLight.b = Clamp(allLight.b, 0.f, 1e5f);

        allLight.r /= 1e5;
        allLight.g /= 1e5;
        allLight.b /= 1e5;
#endif
        return allLight;
    }

    Optional<ShapeIntersection> TestIntegrator10::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }



    RGB TestIntegrator10::Li(const Ray& ray, Float u, Float tMax) const {
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
                        //if (0 != i)
                        //    beta *= 0.6f;
                 
                        // direct light
                        LightSampleContext ctx;
                        ctx.pi = intr->intr.pi;
                        RGB allLight = SampleLd(ctx, intr->intr, ray);
                        beta *= allLight;
                        break;
                    }
                    else if (3 == tag) {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, { 0, 0 });
                        beta *= sample.f;
                        currentRay.d = Normalize(sample.wi);
                        //currentRay.o = intr->intr.p() + currentRay.d;
                        currentRay.o = intr->intr.p();
                    }
                    else {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, { 0, 0 });
                        beta *= sample.f;
                        currentRay.d = Normalize(sample.wi);
                        //currentRay.o = intr->intr.p() + currentRay.d;
                        currentRay.o = intr->intr.p();
                    }
                }
            }
            // ÃÏø’
            else {
                if (scene->hdrLight)
                {
                    auto sample = scene->hdrLight->SampleRay(ray);
                    beta *= sample.L;
                }
                break;
            }
        }
        return beta;
    }

    RGB TestIntegrator10::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

#if 0
        TextureEvalContext ctx;
        ctx.uv = { 1.0f * row / Height, 1.0f * col / Width };
        SampledWavelengths sw;
        RGB c = scene->textures[0].Evaluate(ctx, sw) * 255.f;
        pixel_color[0] = c.r;
        pixel_color[1] = c.g;
        pixel_color[2] = c.b;
        return {};
#endif

        int samples = 4;
        acc_color[0] = 0.f;
        acc_color[1] = 0.f;
        acc_color[2] = 0.f;
        Float uu[] = { 1.0, 0.3 };
        SampledWavelengths lambda;
        for (int i = 0; i < samples; i++) {
            CameraSample cs;
            Point2f p = scene->sampler.Get2D();
            cs.pFilm.x = col + p.x;
            cs.pFilm.y = row + p.y;
            SampledWavelengths w;
            CameraRay r = scene->camera.GenerateRay(cs, w);

            //auto u = RadicalInverse(prim, 2, i);
            //u = 1.0f;
            //RGB final = Li(r.ray, uu[i % 2]);
            Float t = 0.5f;
            Float s = 1.f - t;
            RGB final(0, 0, 0);
            if (IntersetGlass(r.ray)) {
                Float d = 0.5;
                Optional<ShapeIntersection> intr = Intersect(r.ray);
                if (intr) {
                    BSDF bsdf = intr->intr.material.GetBSDF(intr->intr, lambda);
                    BSDFSample sample = bsdf.Sample_f(-r.ray.d, 1.f, {});
                    Ray ray2;
                    ray2.o = intr->intr.p();
                    ray2.d = r.ray.d;
                    Optional<ShapeIntersection> intr2 = Intersect(ray2);
                    if (intr2) {
                        final += sample.f * Li(ray2, 0.f) * t;
                    }
                }
                // ∑¥…‰
                final += Li(r.ray, 0.f) * s;
               
                //final.r = final.r / (final.r + 0.5f);
                //final.g = final.g / (final.g + 0.5f);
                //final.b = final.b / (final.b + 0.5f);

                final.r = Clamp(final.r, 0, 1);
                final.g = Clamp(final.g, 0, 1);
                final.b = Clamp(final.b, 0, 1);
             //   final = Li(r.ray, 0.f) * d + Li(r.ray, 1.f) * (1 - d);
            }
            else {
                final = Li(r.ray, 0.f);
            }
            acc_color[0] += final.r;
            acc_color[1] += final.g;
            acc_color[2] += final.b;
        }

        acc_color[0] = 255 * acc_color[0] / samples;
        acc_color[1] = 255 * acc_color[1] / samples;
        acc_color[2] = 255 * acc_color[2] / samples;

        pixel_color[0] = acc_color[0];
        pixel_color[1] = acc_color[1];
        pixel_color[2] = acc_color[2];
        return {};
    }

    bool TestIntegrator10::IntersetGlass(const Ray& ray, Float tMax) const {
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            auto tag = intr->intr.material.Tag();
            if (3 == tag)
                return true;
        }
        return false;
    }
}