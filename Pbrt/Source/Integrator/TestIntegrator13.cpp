#include <Integrator/TestIntegrator13.h>
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
#include <Math/Noise.h>
// 渲染体积云
// https://www.shadertoy.com/view/ltlSWB 
namespace Render {
    TestIntegrator13::TestIntegrator13(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    Optional<ShapeIntersection> TestIntegrator13::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    // https://www.shadertoy.com/view/MlSSR1
    // https://www.shadertoy.com/view/ltlSWB
    RGB TestIntegrator13::Li(const Ray& ray, Float u, Float tMax) const {

        RGB sky(0, 0, 0);
        if (ray.d.y < 0)
            return sky;
        Vector3f sunDir = Normalize(Vector3f(1, 0.5f, 0));
        Float sundot = Clamp(Dot(ray.d, sunDir), 0.f, 1.f);

        RGB blueSky = RGB(0.3, .55, 0.8);
        RGB redSky = RGB(0.8, 0.8, 0.6);

        sky = Mix(blueSky, redSky, 1.5 * pow(sundot, 8.));
        //sky += RGB(0.9, 0.3, 0.9) * pow(sundot, 0.5)  * 0.1f;
        //sky += RGB(1., 0.7, 0.7)  * pow(sundot, 1.)   * 0.2f;
        sky += RGB(1., 1, 1) * pow(sundot, 256.) * 0.95f;
        // 地平线
        sky = Mix(sky, RGB(0.9, 0.75, 0.8) * 0.9f, pow(1. - max(ray.d.y, 0.f), 8.0));

        sky = Clamp(sky, 0, 1);
        return sky;
    }

    Float TestIntegrator13::Cloud(Point3f p) const {
        Float d = 0.f;
        d += sin(1e-3f * p.x);
        d += cos(1e-4f * p.z);

        // d += scene->sampler.Get1D();

        if (d < 0.0f)
            d = 0.f;

        if (d > 1.f)
            d = 1.f;

        return d;
    }

    Float TestIntegrator13::FBM(Point3f p) const {
        Float sum = 0.f;
        sum += Cloud(p) * 0.5f;
        sum += Cloud(p * 2.f) * 0.25f;
        sum += Cloud(p * 7.f) * 0.125f;
        sum += Cloud(p * 16.f) * 0.0625f;
        return sum;
    }

    RGB TestIntegrator13::LiPixel(int row, int col) const {

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

        RGB color1(0, 0, 0);
        auto intr = Intersect(r.ray);
        if (intr) {
            color1 = intr->intr.material.Evaluate(intr->intr, w);
        }

        RGB color(0, 0, 0);
        Float d = 0.f;
        if (r.ray.d.y > 0) {
            Float h = 1e4f;
            if (h > r.ray.o.y && 0.05 < r.ray.d.y) {
                Float t = (h - r.ray.o.y) / r.ray.d.y;
                Point3f p = r.ray.o + t * r.ray.d;
                //Float d = worleyNoise(p * 1e-3, 4.f);

                Float freq = 1e-5;
                d = cloudShape(p * freq, 0.85f);
                Float steps = 64;
                for (int i = 0; i < steps; i++) {
                    p += 100.f * r.ray.d;
                    d += cloudShape(p * freq, 0.85f);
                }
                d /= steps;
                d = d / (.02f + d);
        
                

                Float c = d;
                color.r = c;
                color.g = c;
                color.b = c;
                
                
            }
        }
        color = Mix(color1, color, d);

        acc_color[0] += color.r;
        acc_color[1] += color.g;
        acc_color[2] += color.b;

        pixel_color[0] = acc_color[0] / scene->SampleCount * 255;
        pixel_color[1] = acc_color[1] / scene->SampleCount * 255;
        pixel_color[2] = acc_color[2] / scene->SampleCount * 255;
        return {};
    }

}