
#include <Integrator/TestIntegrator4.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
#include <Camera/CameraSample.h>
#include <Camera/CameraRay.h>
#include <Intersection/Intersection.h>
#include <Intersection/Interaction.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Math/Scattering.h>
#include <Math/LowDiscrepancy.h>
#include <Weather/Weather.h>
#include <Ray/Ray.h>
#include <Scene/Scene.h>
namespace Render {

    TestIntegrator4::TestIntegrator4(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    Optional<ShapeIntersection> TestIntegrator4::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    RGB TestIntegrator4::Li(const Ray& ray, Float u, Float tMax) const {
        // return  (RGB(0.3412, 0.5020, 0.2353) * 1e4f);
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
#if 1
                        beta *= intr->intr.material.Evaluate(intr->intr, lambda);
                        beta *= 1e5;
#else
                        // BUG
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, { 0, 0 });
                        beta *= sample.f;
#endif
                        // direct light
                        //LightSampleContext ctx;
                        //ctx.pi = intr->intr.pi;
                        //RGB allLight = SampleLd(ctx, intr->intr, ray);
                        //beta *= allLight;
                        break;
                    }
                    else {
                        BSDFSample sample = bsdf.Sample_f(-currentRay.d, u, { 0, 0 });
                        beta *= sample.f;
                        currentRay.d = Normalize(sample.wi);
                        currentRay.o = intr->intr.p() + currentRay.d;
                    }
                }
            }
            else {
                Float s = 1e5;

                RGB sky(0.4902, 0.6588, 0.9373);
                //  RGB sky(0.8667, 0.9529, 1);
                sky *= s;
                beta *= sky;
                break;
            }
        }
        return beta;
    }

    RGB TestIntegrator4::LiPixel(int row, int col) const {

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


        Point2f p = scene->sampler.Get2D();

        CameraSample cs;
        cs.pFilm.x = col + p.x;
        cs.pFilm.y = row + p.y;

        SampledWavelengths w;

        CameraRay r = scene->camera.GenerateRay(cs, w);

        
        // 雾参数
        Float Foggy = 0.85f;
        Float d = 1e5;
        Float D = 2e5;

        RGB FinalColor(0, 0, 0);
        RGB Beta(1.f, 1.f, 1.f);
        Ray ray = r.ray;
        Float u = scene->sampler.Get1D();
        u = 0.f;
        if (scene->SampleCount % 2 == 0)
            u = 1.f;
        for (int i = 0; i < 5; i++) {
            Optional<ShapeIntersection> intr = Intersect(ray);
            if (intr) {
                // 读取法线贴图
                BSDF bsdf = intr->intr.GetBSDF();

                //BSDF bsdf = intr->intr.material.GetBSDF(intr->intr, w);

                //Float u = scene->sampler.Get1D();

                BSDFSample Sample = bsdf.Sample_f(-ray.d, u, { 0, 0 });
                RGB Color = intr->intr.material.Evaluate(intr->intr, w);
                Beta *= Color * Sample.f;

                ray.o = intr->intr.p();
                ray.d = Sample.wi;

                int tag = intr->intr.material.Tag();
                if (1 == tag) 
                    break;

                u = 1.0f;
                
            }
            else {
                Beta *= 0.f;
                if (r.ray.d.y > 0.f) {
                    Beta *= scene->weather->SkyColor(r.ray.d);
                }
                break;
            }
        }


        //d = 0;
        //Foggy = d / D;
        //FinalColor = Mix(FinalColor, RGB(1, 1, 1), Foggy);
        FinalColor = Clamp(Beta, 0.f, 1.f);

        acc_color[0] += FinalColor.r;
        acc_color[1] += FinalColor.g;
        acc_color[2] += FinalColor.b;

        pixel_color[0] = acc_color[0] / scene->SampleCount * 255;
        pixel_color[1] = acc_color[1] / scene->SampleCount * 255;
        pixel_color[2] = acc_color[2] / scene->SampleCount * 255;
        return {};
    }


}