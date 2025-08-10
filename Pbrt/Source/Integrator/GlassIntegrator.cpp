#include <Integrator/GlassIntegrator.h>
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
#include <Shape/Sphere.h>

namespace Render {
    GlassIntegrator::GlassIntegrator(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    Optional<ShapeIntersection> GlassIntegrator::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    RGB GlassIntegrator::Li(const Ray& ray, Float u, Float tMax) const {
        // return  RGB(0.3412, 0.5020, 0.2353);
        RGB color(0, 0, 0);
        RGB beta(1, 1, 1);
        // Ray r = ray;
        SampledWavelengths lambda;

        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            auto tag = intr->intr.material.Tag();
            if (tag > 0) {

                RGB beta = intr->intr.material.Evaluate(intr->intr, lambda);
                color = beta;
            }
        }
        else {
            // 天空
            if (ray.d.y > 0) {
                color.r = 0.6;
                color.g = 0.851;
                color.b = 0.9176;
            }
        }

        return color;
    }

    RGB GlassIntegrator::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];

        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        int samples = 1;
        if (1 == scene->SampleCount) {
            acc_color[0] = 0.f;
            acc_color[1] = 0.f;
            acc_color[2] = 0.f;
        }
        // 1
        //RGB sky(0.60f, 0.7333f, 0.9276f);
        // 2
       // RGB sky(0.4588f, 0.6863f, 0.9373f);
        RGB sky(0.4431f, 0.6431f, 0.9176);
        //RGB sky(1, 1, 1);
        CameraSample cs;
        SampledWavelengths w;
        for (int i = 0; i < samples; i++) {
            auto p = scene->sampler.Get2D();
            
            cs.pFilm.x = col + p.x;
            cs.pFilm.y = row + p.y;

            //cs.pFilm.x = col;
            //cs.pFilm.y = row;
            RGB c(0, 0, 0);
            CameraRay r = scene->camera.GenerateRay(cs, w);
            
            Optional<ShapeIntersection> intr = Intersect(r.ray);
            //if (1 == row && ((Width - 1)== col)) {
            //    printf("ray %f %f %f %d\n", r.ray.d.x, r.ray.d.y, r.ray.d.z, Width);
            //}
            RGB tmp1 = sky;
            //if (r.ray.d.x > 0)
            //    tmp1 *= 0.6;
            
            if (intr) {
                c = intr->intr.material.Evaluate(intr->intr, w);
                // 玻璃
                if (3 == intr->intr.material.Tag()) {
                    Ray ray;
                    RGB glassColor = c;
                    RGB T(0, 0, 0);
                    RGB R(0, 0, 0);
                    // 透射
                    ray.o = intr->intr.p();
                    ray.d = r.ray.d;

                    // 1
                    // Float ts = 1.0f;

                    // 2
                    // Float ts = 1.0f;
                    
                    // 3
                    Float ts = 1.0f;

                    Optional<ShapeIntersection> intr2 = Intersect(ray);
                    if (intr2) {

                        T = intr2->intr.material.Evaluate(intr2->intr, w) * ts;
                    }

                    ray.d = Reflect(-r.ray.d, intr->intr.shading.n);
                    RGB tmp = sky;
                    // 1
                    //Float rs = 0.95f;

                    // 2
                    Float rs = 0.95f;

                    // 3
                    //Float rs = 0.95f;

                    Optional<ShapeIntersection> intr3 = Intersect(ray);
                    if (intr3) {
 
                        R = intr3->intr.material.Evaluate(intr3->intr, w);
                        // 1
                        // s = 0.2f;
                        
                        // 2
                        // rs = 0.55f;

                        // 2
                        rs = 0.55f;
                    }
                    else {
                        R = tmp;
                        // 1
                        //rs = 0.92f;

                        // 2
                        //rs = Clamp(ray.d.y * 1.5f, 0.f, 1.0f);

                        // 3
                        rs = Clamp(ray.d.y * 1.5f, 0.f, 1.0f);
                        //rs = Remap(rs, 0.f, 1.f, 0.5f, 1.0f);
     
                    }
                    c = Mix(glassColor * T, R, rs);
                }
            }
            else {
                c = tmp1;
            }

            acc_color[0] += c.r;
            acc_color[1] += c.g;
            acc_color[2] += c.b;  
        }
        //pixel_color[0] = acc_color[0] / samples * 255;
        //pixel_color[1] = acc_color[1] / samples * 255;
        //pixel_color[2] = acc_color[2] / samples * 255;

        pixel_color[0] = acc_color[0] / scene->SampleCount * 255;
        pixel_color[1] = acc_color[1] / scene->SampleCount * 255;
        pixel_color[2] = acc_color[2] / scene->SampleCount * 255;

        //if (col > Width / 2) {
        //    pixel_color[0] = 0.f;
        //    pixel_color[1] = 0.f;
        //    pixel_color[2] = 0.f;
        //}

        return {};
    }

}