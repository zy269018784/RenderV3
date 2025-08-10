#include <Integrator/TestIntegrator3.h>
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
#include <Math/EqualAreaMapping.h>
#include <Light/HDRLight.h>
namespace Render {
    TestIntegrator3::TestIntegrator3(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    bool TestIntegrator3::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;
        if (intr && intr->tHit < tMax)
            return true;
        return false;
    };

    bool TestIntegrator3::Unoccluded(const Interaction& p0, const Interaction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();

        // Float tMax = Length(p1.p() - p0.p());
        // return !IntersectP(ray, tMax);
        return !IntersectP(ray, Infinity);
    };

    bool TestIntegrator3::IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr && intr->tHit < tMax && intr->intr.material.Tag() == 3) {
            SampledWavelengths lambda;
            //beta = intr->intr.material.GetBSDF(intr->intr, lambda).Sample_f(-ray.d, 1.0f, {}).f;

            beta = intr->intr.material.Evaluate(intr->intr, lambda);
            return true;
        }
        return false;
    };

    bool TestIntegrator3::ThroughTransparent(const Interaction& p0, const Interaction& p1, RGB& beta)  const {
        Ray ray;
        ray.o = p0.p();
        ray.d = Normalize(p1.p() - p0.p());
        Float tMax = Length(p1.p() - p0.p());
        bool ret = IntersectTransparent(ray, tMax, beta);
        //beta *= 1.f / (1.f + tMax);
        return ret;
    };

    CPU_GPU RGB TestIntegrator3::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray) const {
        RGB allLight(0.4e5, 0.4e5, 0.4e5);
        //RGB allLight(0.f, 0.f, 0.f);
         // 光照点的材质
        Material m = intr.material;
        // 光照点的BSDF
        SampledWavelengths lambda;
        BSDF bsdf = m.GetBSDF(intr, lambda);
        // 光照点的局部坐标
        Vector3f wo = bsdf.ToLocal(-ray.d);
        for (int i = 0; i < scene->nLight; i++) {
            LightLiSample lightSample = scene->light[i].SampleLi(ctx, {}, {});
            // 光照点的局部坐标
            Vector3f wi = bsdf.ToLocal(lightSample.wi);
            if (!SameHemisphere(wo, wi)) {
                continue;
            }

            //Ray testRay;
            //testRay.d = Normalize(lightSample.pLight.p() - ctx.p());
            //testRay.o = intr.p() - testRay.d;
            //
            //Ray testRay2;
            //testRay2.d = Normalize(lightSample.pLight.p() - ctx.p());
            //testRay2.o = intr.p() + testRay2.d;
            //
            //Optional<ShapeIntersection> testIntr = Intersect(testRay);
            //Optional<ShapeIntersection> testIntr2 = Intersect(testRay2);
            //
            //if (testIntr2 && !testIntr.has_value()) {
            //    continue;
            //}

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
            //allLight += beta;
        }
#if 0
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

    Optional<ShapeIntersection> TestIntegrator3::Intersect(const Ray& ray,
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


    RGB TestIntegrator3::Li(const Ray& ray, Float u, Float tMax) const {
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
                        // beta *= 0.5;
                        // direct light
                        LightSampleContext ctx;
                        ctx.pi = intr->intr.pi;
#if 1
                        RGB AllLight(0, 0, 0);
                        Float samples = 10.f;
                        Float nrSamples = 0.f;
                        if (scene->hdr_image) {
                            for (int n = 0; n < samples; n++) {
                                auto u2 = scene->sampler.Get2D();
                                //auto u2 = scene->u[n];
                                Point2i offset;
                                // auto xy = scene->pc2d->Sample(u, nullptr, &offset);
                                auto uv = scene->pc2d->Sample(u2, nullptr, &offset);
                                AllLight.r += scene->hdr_image->GetChannel(offset, 0);
                                AllLight.g += scene->hdr_image->GetChannel(offset, 1);
                                AllLight.b += scene->hdr_image->GetChannel(offset, 2);
#if 0
                                Vector3f wLight = EqualAreaSquareToSphere(uv);
                                Vector3f wi = bsdf.ToLocal(wLight);
                                Vector3f wo = bsdf.ToLocal(-currentRay.d);
                                if (SameHemisphere(wo, wi)) {
                                    AllLight.r += scene->hdr_image->GetChannel(offset, 0);
                                    AllLight.g += scene->hdr_image->GetChannel(offset, 1);
                                    AllLight.b += scene->hdr_image->GetChannel(offset, 2);
                                    nrSamples += 1.f;
                                }
#endif
                            }
                        }
                        AllLight /= samples;
                        //printf("%f %f %f\n", AllLight.r, AllLight.g, AllLight.b);
                        AllLight = Clamp(AllLight, 0, 1);
                        beta *= AllLight;
#endif
                        //RGB allLight = SampleLd(ctx, intr->intr, ray);
                        //beta *= allLight;
                        break;
                    }
                    else if (2 == tag) {
                        Point2f u2 = scene->sampler.Get2D();
                        //Point2f u2(std::sin(scene->SampleCount * 31233.413f) * 43758.5453123f, std::cos(scene->SampleCount * 43255.f) * 43758.5453123f);
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, u2);
                        beta *= intr->intr.material.Evaluate(intr->intr, lambda);
                        beta *= sample.f;
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
                        break;
                    }
                    else if (3 == tag) {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, { 0, 0 });
                        beta *= sample.f;
                        if (u > 0.5f)
                        {
                            auto glassColor = intr->intr.material.Evaluate(intr->intr, lambda);
                            beta *= glassColor;
                        }
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p();
                    }
                    else {
                        Point2f u2 = scene->sampler.Get2D();
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


                auto sample = scene->hdrLight->SampleRay(currentRay);
                sky = sample.L;

                beta *= sky;
                break;
            }
        }
        return beta;
    }

    RGB TestIntegrator3::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        // pixel_color[0] = 0.5137 * 255; 
        // pixel_color[1] = 0.7373 * 255;
        // pixel_color[2] = 0.9059 * 255;
        // return {};

        if (1 == scene->SampleCount) {
            acc_color[0] = 0.f;
            acc_color[1] = 0.f;
            acc_color[2] = 0.f;
        }

        CameraSample cs;
        Point2f p = scene->sampler.Get2D();
        cs.pFilm.x = col + p.x;
        cs.pFilm.y = row + p.y;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        RGB final(0, 0, 0);
        if (IntersetGlass(r.ray))
        {
            final = Li(r.ray, 0.f) + Li(r.ray, 1.f);
            //  final = ACESFilm(final);
        }
        else {
            final = Li(r.ray, 0.f);
        }
        final = ACESFilm(final);

        acc_color[0] += final.r;
        acc_color[1] += final.g;
        acc_color[2] += final.b;

        pixel_color[0] = acc_color[0] / scene->SampleCount * 255;
        pixel_color[1] = acc_color[1] / scene->SampleCount * 255;
        pixel_color[2] = acc_color[2] / scene->SampleCount * 255;

        return {};
    }

    bool TestIntegrator3::IntersetGlass(const Ray& ray, Float tMax) const {
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            auto tag = intr->intr.material.Tag();
            if (3 == tag)
                return true;
        }
        return false;
    }
}