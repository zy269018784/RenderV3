#include <Integrator/TestIntegrator11.h>
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
namespace Render {
    // äÖÈ¾Ì«Ñô
    TestIntegrator11::TestIntegrator11(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    bool TestIntegrator11::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;
        if (intr && intr->tHit < tMax)
            return true;
        return false;
    };

    bool TestIntegrator11::Unoccluded(const Interaction& p0, const Interaction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();

        // Float tMax = Length(p1.p() - p0.p());
        // return !IntersectP(ray, tMax);
        return !IntersectP(ray, Infinity);
    };

    bool TestIntegrator11::IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr && intr->tHit < tMax && intr->intr.material.Tag() == 3) {
            SampledWavelengths lambda;
            //beta = intr->intr.material.GetBSDF(intr->intr, lambda).Sample_f(-ray.d, 1.0f, {}).f;

            beta = intr->intr.material.Evaluate(intr->intr, lambda);
            return true;
        }
        return false;
    };

    bool TestIntegrator11::ThroughTransparent(const Interaction& p0, const Interaction& p1, RGB& beta)  const {
        Ray ray;
        ray.o = p0.p();
        ray.d = Normalize(p1.p() - p0.p());
        Float tMax = Length(p1.p() - p0.p());
        bool ret = IntersectTransparent(ray, tMax, beta);
        //beta *= 1.f / (1.f + tMax);
        return ret;
    };

    CPU_GPU RGB TestIntegrator11::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray) const {
        RGB allLight(0.4f, 0.4f, 0.4f);
        //RGB allLight(0.7f, 0.7f, 0.7f);
        // RGB allLight(7e4f, 7e4f, 7e4f);
       // RGB allLight(0.f, 0.f, 0.f);
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

    Optional<ShapeIntersection> TestIntegrator11::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    // https://www.shadertoy.com/view/MlSSR1
    // https://www.shadertoy.com/view/ltlSWB
    RGB TestIntegrator11::Li(const Ray& ray, Float u, Float tMax) const {

        RGB sky(0, 0, 0);
        if (ray.d.y < 0)
            return sky;
        Vector3f sunDir = Normalize(Vector3f(1, 0.2f, 0));
        Float sundot = Clamp(Dot(ray.d, sunDir), 0.f, 1.f);

        RGB blueSky = RGB(0.3, .55, 0.8);
        RGB redSky  = RGB(0.8, 0.8, 0.6);
     
        //sky = Mix(blueSky, redSky, 1.5 * pow(sundot, 8.));
        //sky += RGB(0.9f, 0.3f, 0.9f) * pow(sundot, 0.5f) * 0.1f;
        //sky += RGB(1., 0.7, 0.7)  * pow(sundot, 1.)   * 0.2f;
        //sky += RGB(1., 1, 1) * pow(sundot, 256.) * 0.95f;
        //sky = Mix(sky, RGB(0.9f, 0.3f, 0.9f),  pow(sundot, 128.f));
        //sky = Mix(sky, RGB(1.0f, 1.0f, 1.0f),  pow(sundot, 256.f));
        //sky += RGB(0.9f, 0.3f, 0.9f) * pow(sundot, 200);
        //sky += RGB(1.f, 1.f, 1.f) * pow(sundot, 256.f);


       // sky = RGB(0.8, 0.8, 0.6) * 1.5f * pow(sundot, 8.f * 8.f);
        sky = Mix(blueSky, redSky, 1.5 * pow(sundot, 8.f));

        sky = sky * (1.0f - 0.8f * ray.d.y);
        sky += RGB(1., 0.7, 0.7) * pow(sundot, 1.f) * .2f;
        sky += RGB(1.f, 1.f, 1.f) * pow(sundot, 256.f) * 0.95f;

        // µØÆ½Ïß
        sky = Mix(sky, RGB(0.9f, 0.75f, 0.8f) * 0.9f, pow(1.f - max(ray.d.y + 0.1f, 0.f), 8.0f));

        sky = Clamp(sky, 0, 1);
        return sky;
    }

    RGB TestIntegrator11::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        int samples = 4;
        acc_color[0] = 0.f;
        acc_color[1] = 0.f;
        acc_color[2] = 0.f;
        for (int i = 0; i < samples; i++) {
            CameraSample cs;
            Point2f p = scene->sampler.Get2D();
            cs.pFilm.x = col + p.x;
            cs.pFilm.y = row + p.y;
            SampledWavelengths w;
            CameraRay r = scene->camera.GenerateRay(cs, w);

            RGB final = Li(r.ray, 0.f);
        
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

    bool TestIntegrator11::IntersetGlass(const Ray& ray, Float tMax) const {
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            auto tag = intr->intr.material.Tag();
            if (3 == tag)
                return true;
        }
        return false;
    }
}