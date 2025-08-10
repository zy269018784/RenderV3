#include <Integrator/TestIntegrator12.h>
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
    TestIntegrator12::TestIntegrator12(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    bool TestIntegrator12::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;
        if (intr && intr->tHit < tMax)
            return true;
        return false;
    };

    bool TestIntegrator12::Unoccluded(const Interaction& p0, const Interaction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();

        // Float tMax = Length(p1.p() - p0.p());
        // return !IntersectP(ray, tMax);
        return !IntersectP(ray, Infinity);
    };

    bool TestIntegrator12::IntersectTransparent(const Ray& ray, Float tMax, RGB& beta) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr && intr->tHit < tMax && intr->intr.material.Tag() == 3) {
            SampledWavelengths lambda;
            //beta = intr->intr.material.GetBSDF(intr->intr, lambda).Sample_f(-ray.d, 1.0f, {}).f;

            beta = intr->intr.material.Evaluate(intr->intr, lambda);
            return true;
        }
        return false;
    };

    bool TestIntegrator12::ThroughTransparent(const Interaction& p0, const Interaction& p1, RGB& beta)  const {
        Ray ray;
        ray.o = p0.p();
        ray.d = Normalize(p1.p() - p0.p());
        Float tMax = Length(p1.p() - p0.p());
        bool ret = IntersectTransparent(ray, tMax, beta);
        //beta *= 1.f / (1.f + tMax);
        return ret;
    };

    CPU_GPU RGB TestIntegrator12::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray) const {
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

    Optional<ShapeIntersection> TestIntegrator12::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    // https://www.shadertoy.com/view/MlSSR1
    // https://www.shadertoy.com/view/ltlSWB
    RGB TestIntegrator12::Li(const Ray& ray, Float u, Float tMax) const {

        RGB sky(0, 0, 0);
        if (ray.d.y < 0)
            return sky;
        Vector3f sunDir = Normalize(Vector3f(1, 0.5f, 0));
        Float sundot = Clamp(Dot(ray.d, sunDir), 0.f, 1.f);

        RGB blueSky = RGB(0.3, .55, 0.8);
        RGB redSky  = RGB(0.8, 0.8, 0.6);
     
        sky = Mix(blueSky, redSky, 1.5 * pow(sundot, 8.));
        //sky += RGB(0.9, 0.3, 0.9) * pow(sundot, 0.5)  * 0.1f;
        //sky += RGB(1., 0.7, 0.7)  * pow(sundot, 1.)   * 0.2f;
        sky += RGB(1., 1, 1)      * pow(sundot, 256.) * 0.95f;
        // 地平线
        sky = Mix(sky, RGB(0.9, 0.75, 0.8) * 0.9f, pow(1. - max(ray.d.y, 0.f), 8.0));

        sky = Clamp(sky, 0, 1);
        return sky;
    }

    RGB TestIntegrator12::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        Float u = row * 1.f / Width  * 2.f;
        Float v = col * 1.f / Height * 2.f;


       // Float cloud = worleyNoise(Vector3f(u, v, 0.f), 4.f) * 255.f;
       // Float cloud = gradientNoise(Vector3f(u, v, 0.f), 4.f) * 255.f;
       // Float cloud = perlinfbm(Vector3f(u, v, 0.f), 4.f, 7) * 255.f;
       // Float cloud = worleyFbm(Vector3f(u, v, 0.f), 4.f) * 255.f;
       // Float cloud = cloudShape(Vector3f(u, v, 0.f), 0.85f) * 255.f;
        Vector3f AA = Vector3f(-4000000,   120000.f, -4000000);
        Vector3f BB = Vector3f( 4000000,  1120000.f,  4000000);
        
        
        CameraSample cs;
        cs.pFilm.x = col;
        cs.pFilm.y = row;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);
       
        Vector2f hit = intersectAABB(r.ray.o, r.ray.d, AA, BB);
        Float cloud = 0; 
        
        if (0 == row && 0 == col)
            printf("camera pos %f %f %f\n", r.ray.o.x, r.ray.o.y, r.ray.o.z);
#if 1
        if (hit.y > -1.) {
            // AABB和 2个交点的距离
            Float t0 = fmaxf(hit.x, 0.);
            Float t1 = hit.y;
            // 步进步数
            Float steps = 256;
            // 步长距离
            Float step_size = (t1 - t0) / steps;
            // 步进距离
            Vector3f step_dir = step_size * r.ray.d;
            // 当前位置
            Vector3f pos = r.ray.o + t0 * r.ray.d;
            // 积累的密度
            Float d = 0.f;
            // 云数量
            Float c = 0.9f;
            for (int i = 0; i < steps; i++)
            {
                Vector3f uvw = (pos - AA) / (BB - AA);
                //d += cloudShape(Vector3f(uvw.x * 5.f, uvw.z * 5.f, uvw.y), c);
                d += cloudShape(Vector3f(uvw.x * .5f, uvw.z * .5f, uvw.y), c);
                pos += step_dir;
            }

            cloud = d / (1.f + d) * 255;
        }
#else
        if (r.ray.d.y > 0)
        {
            // AABB和 2个交点的距离
            Float t0 = 15e4;
            t0 = fminf(15e4, r.ray.o.y);
            Float t1 = t0 + 100e4;
            // 步进步数
            Float steps = 32;
            // 步长距离
            Float step_size = (t1 - t0) / steps / r.ray.d.y;
            // 步进距离
            Vector3f step_dir = step_size * r.ray.d;
            // 当前位置
            Vector3f pos = r.ray.o + ((t0  - r.ray.o.y) / r.ray.d.y) * r.ray.d;
            // 积累的密度
            Float d = 0.f;
            // 云数量
            Float c = 0.95f;

            Float range = 2e6;
            for (int i = 0; i < steps; i++)
            {
                Vector3f uvw = Vector3f(fmodf(pos.x, range) / range, fmodf(pos.y, 1e4) / 1e4, fmodf(pos.z, range) / range);
                d += cloudShape(Vector3f(uvw.x, uvw.z, uvw.y), c);
                pos += step_dir;
            }
            cloud = d / (1.f + d) * 255;       
        }
#endif
        pixel_color[0] = cloud;
        pixel_color[1] = cloud;
        pixel_color[2] = cloud;
        return {};
    }

    bool TestIntegrator12::IntersetGlass(const Ray& ray, Float tMax) const {
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            auto tag = intr->intr.material.Tag();
            if (3 == tag)
                return true;
        }
        return false;
    }
}