#include <Integrator/TestHDR.h>
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
#include <Math/SampleSphericalMap.h>
#include <Light/HDRLight.h>
namespace Render {
    TestHDR::TestHDR(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    bool TestHDR::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;
        if (intr && intr->tHit < tMax)
            return true;
        return false;
    };

    bool TestHDR::Unoccluded(const Interaction& p0, const Interaction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();

        // Float tMax = Length(p1.p() - p0.p());
        // return !IntersectP(ray, tMax);
        return !IntersectP(ray, Infinity);
    };

    bool TestHDR::IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr && intr->tHit < tMax && intr->intr.material.Tag() == 3) {
            SampledWavelengths lambda;
            //beta = intr->intr.material.GetBSDF(intr->intr, lambda).Sample_f(-ray.d, 1.0f, {}).f;

            beta = intr->intr.material.Evaluate(intr->intr, lambda);
            return true;
        }
        return false;
    };

    bool TestHDR::ThroughTransparent(const Interaction& p0, const Interaction& p1, RGB& beta)  const {
        Ray ray;
        ray.o = p0.p();
        ray.d = Normalize(p1.p() - p0.p());
        Float tMax = Length(p1.p() - p0.p());
        bool ret = IntersectTransparent(ray, tMax, beta);
        //beta *= 1.f / (1.f + tMax);
        return ret;
    };

    CPU_GPU RGB TestHDR::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray) const {
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
            allLight += beta;
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

    Optional<ShapeIntersection> TestHDR::Intersect(const Ray& ray,
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


    RGB TestHDR::Li(const Ray& ray, Float u, Float tMax) const {
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
                        RGB allLight = SampleLd(ctx, intr->intr, ray);
                        beta *= allLight;
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

    RGB  TestHDR::GetIrradiance(Point3f p) const {
        Ray ray;
        ray.o = p;
        RGB irradiance(0.f, 0.f, 0.f);
        Float sampleDelta = 0.025;
       // Float sampleDelta = 2.5f;
        Float nrSamples = 0.0f;
        for (Float phi = 0.0; phi < 2.0 * Pi; phi += sampleDelta) {
            for (Float theta = 0.0; theta < 0.5 * Pi; theta += sampleDelta) {
                ray.d.y = Cos(theta);
                ray.d.x = Sin(theta) * Cos(phi);
                ray.d.z = Sin(theta) * Sin(phi);
                auto intr = Intersect(ray);
                nrSamples++;
                if (intr) {
                    //intr.reset();
                    break;
                }
                auto sample = scene->hdrLight->SampleRay(ray);
                irradiance += sample.L; 
            }
        }
        irradiance = Pi * irradiance * (1.0 / float(nrSamples));
        return irradiance;
    }

    RGB TestHDR::LiPixel(int row, int col) const {
        //if (row != 300 && col != 400)
        //    return {};

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
        auto intr = Intersect(r.ray);
        if (intr) {
            final = intr->intr.material.Evaluate(intr->intr, w);

            Point2i offset;
            RGB L(0, 0, 0);
            // 采样10次HDR
            for (int i = 0; i < 10; i++) {

                Point2f u2 = scene->sampler.Get2D();
                //u2 = Point2f(0.9f, 0.9f);
                //Point2f u2 = scene->u[i];

                // 采样HDR
                Point2f xy = scene->pc2d->Sample(u2, nullptr, &offset);

                // HDR uv
                Point2f uv(xy.x, 1.f - xy.y);
                //uv = xy;

                // uv转dir
                Vector3f d = Normalize(InvertSampleSphericalMap(uv));
                //d = Reflect(d, intr->intr.n);

                Ray ray;
                ray.o = intr->intr.p();
                ray.d = d;
                auto intrShadow = Intersect(ray);
                if (!intrShadow) {
                    Point2i p;
                    p.x = uv.x * scene->hdr_image->Resolution().x;
                    p.y = (1 - uv.y) * scene->hdr_image->Resolution().y;

                    RGB light;
                    light.r = scene->hdr_image->GetChannel(p, 0);
                    light.g = scene->hdr_image->GetChannel(p, 1);
                    light.b = scene->hdr_image->GetChannel(p, 2);
                    light = ACESFilm(light);
                    //p = offset;
                    // 积累光照
                    L.r += light.r;
                    L.g += light.g;
                    L.b += light.b;
                }
            }
           // L /= 10.f;
            L.r = Clamp(L.r, 0.0f, 1.f);
            L.g = Clamp(L.g, 0.0f, 1.f);
            L.b = Clamp(L.b, 0.0f, 1.f);

            final *= L;

#if 0
            auto u2 = scene->sampler.Get2D();
            Point2i offset;
            // 采样HDR
            auto xy = scene->pc2d->Sample(u2, nullptr, &offset);
            // HDR uv
            Point2f uv(xy.x, 1.f - xy.y);

            Vector3f d = Normalize(InvertSampleSphericalMap(uv));
            //printf("uv %f %f, d %f %f %f, offset %d %d\n", uv.x, uv.y, d.x, d.y, d.z, offset.x, offset.y);
            Ray ray;
            ray.o = intr->intr.p();
            ray.d = d;
            auto intrShadow = Intersect(ray);
            if (!intrShadow)
            {
                // HDR spherical
                //uv = SampleSphericalMap(d);

                Point2i p;
                p.x = uv.x * scene->hdr_image->Resolution().x;
                p.y = (1 - uv.y) * scene->hdr_image->Resolution().y;

                final = intr->intr.material.Evaluate(intr->intr, w);
                // 获取HDR像素值
                final.r *= Clamp(scene->hdr_image->GetChannel(p, 0), 0, 1);
                final.g *= Clamp(scene->hdr_image->GetChannel(p, 1), 0, 1);
                final.b *= Clamp(scene->hdr_image->GetChannel(p, 2), 0, 1);
            }
#endif

        }
        else {
            auto sample = scene->hdrLight->SampleRay(r.ray);
           //final = ACESFilm(sample.L);
            final = sample.L;
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

    bool TestHDR::IntersetGlass(const Ray& ray, Float tMax) const {
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            auto tag = intr->intr.material.Tag();
            if (3 == tag)
                return true;
        }
        return false;
    }
}