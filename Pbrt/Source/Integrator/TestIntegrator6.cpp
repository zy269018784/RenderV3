#include <Integrator/TestIntegrator6.h>
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
    TestIntegrator6::TestIntegrator6(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    bool TestIntegrator6::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;
        if (intr && intr->tHit < tMax)
            return true;
        return false;
    };

    bool TestIntegrator6::Unoccluded(const SurfaceInteraction& p0, const SurfaceInteraction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();
        
        // Float tMax = Length(p1.p() - p0.p());
        // return !IntersectP(ray, tMax);
        return !IntersectP(ray, Infinity);
    };

    bool TestIntegrator6::IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr && intr->tHit < tMax && intr->intr.material.Tag() == 3) {
            SampledWavelengths lambda;
            //beta = intr->intr.material.GetBSDF(intr->intr, lambda).Sample_f(-ray.d, 1.0f, {}).f;

            beta = intr->intr.material.Evaluate(intr->intr, lambda);
            return true;
        }
        return false;
    };

    bool TestIntegrator6::ThroughTransparent(const SurfaceInteraction& p0, const SurfaceInteraction& p1, RGB& beta)  const {
        Ray ray;
        ray.o = p0.p();
        ray.d = Normalize(p1.p() - p0.p());
        Float tMax = Length(p1.p() - p0.p());
        bool ret = IntersectTransparent(ray, tMax, beta);
        //beta *= 1.f / (1.f + tMax);
        return ret;
    };

    CPU_GPU RGB TestIntegrator6::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray) const {
        RGB allLight(0.4f, 0.4f, 0.4f);
        //RGB allLight(0.2f, 0.2f, 0.2f);
        //RGB allLight(0.f, 0.f, 0.f);
        SampledWavelengths lambda;
        for (int i = 0; i < scene->nLight; i++) {
            LightLiSample lightSample = scene->light[i].SampleLi(ctx, {}, {});
#if 0
            Ray currentRay;
            currentRay.o = ctx.p();
            currentRay.d = lightSample.wi;
            Float tMax = Length(lightSample.pLight.p() - ctx.p());
            //RGB beta(1, 1, 1);
            RGB beta = intr.material.Evaluate(intr, lambda);
           // beta *= lightSample.L;
            int depth = 2;
            while(1) {
                //if (depth == 0) {
                //    break;
                //}
                //depth--;
                //Optional<ShapeIntersection> si = Intersect(currentRay, tMax);
                Optional<ShapeIntersection> si = Intersect(currentRay);
                if (si) {
                    break;
                    auto tag = intr.material.Tag();
                   /**if (1 == tag) {
                        beta *= si->intr.material.Evaluate(si->intr, lambda);
                        //beta *= lightSample.L;
                        //break;
                    }
                    else*/
                    if (3 == tag) {
                        //LightSampleContext currentCtx;
                        //currentCtx.pi = si->intr.p();
                        //LightLiSample currentLightSample = scene->light[i].SampleLi(currentCtx, {}, {});

                        currentRay.o = si->intr.p();
                        tMax = Length(lightSample.pLight.p() - si->intr.p());

                        //beta1 *= si->intr.material.Evaluate(si->intr, {});
                        BSDF bsdf = si->intr.material.GetBSDF(si->intr, lambda);
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, 1.0f, { 0, 0 });
                        beta *= sample.f;
                        //beta *= lightSample.L;
                        //break;
                    } 
                    else {
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
#else
            RGB beta(0, 0, 0);
            if (ThroughTransparent(intr, lightSample.pLight, beta))
                allLight += beta * lightSample.L;
            else if (Unoccluded(intr, lightSample.pLight))
                allLight += lightSample.L;
#endif
        }

        allLight.r /= (1 + allLight.r);
        allLight.g /= (1 + allLight.g);
        allLight.b /= (1 + allLight.b);

        return allLight;
    }

    Optional<ShapeIntersection> TestIntegrator6::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate) { return aggregate.Intersect(ray, tMax);  }
        return {};
    }

    RGB TestIntegrator6::Li(const Ray& ray, Float u, Float tMax) const {
       // return  (RGB(0.3412, 0.5020, 0.2353) * 1e4f);
        RGB beta(1, 1, 1);
        int depth = 5;
        Ray currentRay = ray;
        for (int i = 0; i < depth; i++) {
            Optional<ShapeIntersection> intr = Intersect(currentRay, tMax);
            if (intr) {
               // printf("uv %f %f\n", intr->intr.uv.x, intr->intr.uv.y);
                auto tag = intr->intr.material.Tag();
                if (tag > 0) {
                    SampledWavelengths lambda;
                    BSDF bsdf = intr->intr.material.GetBSDF(intr->intr, lambda);
                    if (1 == tag) {

                        beta *= intr->intr.material.Evaluate(intr->intr, lambda);
                     //   beta *= 1e5;

                        // direct light
                        LightSampleContext ctx;
                        ctx.pi = intr->intr.pi;
                        RGB allLight = SampleLd(ctx, intr->intr, ray);
                        beta *= allLight * 1e5;
                        break;
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
            else { 
                Float s = 1e5;
           
                RGB sky(0.4902, 0.6588, 0.9373);
                //RGB sky(0.8667, 0.9529, 1);
                sky *= s;
                beta *= sky; 
                break;
            }
        }
        return beta;
    }

    RGB TestIntegrator6::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];
        
        //if (400 == col && 300 == row)
        {
            CameraSample cs;
            cs.pFilm.x = col + 0.5f;
            cs.pFilm.y = row + 0.5f;
            SampledWavelengths w;
            CameraRay r = scene->camera.GenerateRay(cs, w);
            Optional<ShapeIntersection> intr = Intersect(r.ray);
            RGB c(0, 0, 0);
            if (intr) {
                c = intr->intr.material.Evaluate(intr->intr, w) * 255;
                auto n = intr->intr.shading.n;
                // 获取法线贴图
                intr->intr.GetBSDF();
                Float s = Dot(intr->intr.shading.n, Normal3f(0, -1, 0));
                c *= s;
                //printf("intr.n %f %f %f, %f %f %f\n", n.x, n.y, n.z, intr->intr.n.x, intr->intr.n.y, intr->intr.n.z);
            }
            pixel_color[0] = c.r;
            pixel_color[1] = c.g;
            pixel_color[2] = c.b;

            //pixel_color[0] = 255;
            //pixel_color[1] = 0;
            //pixel_color[2] = 255;
        }
        return {};
    }
}