#include <Integrator/NullIntegrator.h>
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
    NullIntegrator::NullIntegrator(Primitive aggregate, Scene* scene)  {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    Optional<ShapeIntersection> NullIntegrator::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    static CPU_GPU float to_sRGB(float rgb_color)
    {
        const double a = 0.055;
        if (rgb_color < 0.0031308)
            return 12.92 * rgb_color;
        else
            return (1.0 + a) * std::pow(rgb_color, 1.0 / 2.4) - a;
    }
   RGB NullIntegrator::Li(const Ray& ray, Float u, Float tMax) const {
       // return  RGB(0.3412, 0.5020, 0.2353);
        RGB color(0, 0, 0);
        RGB beta(1, 1, 1);
       // Ray r = ray;
        SampledWavelengths lambda;
    
        Optional<ShapeIntersection> intr = Intersect(ray, tMax);
        if (intr) {
            auto tag = intr->intr.material.Tag();
            if (tag > 0) {
                //printf("n %f %f %f\n", intr->intr.n.x, intr->intr.n.y, intr->intr.n.z);
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

   static CPU_GPU RGB ACESFilm(RGB x)
   {
       float a = 2.51f;
       float b = 0.03f;
       float c = 2.43f;
       float d = 0.59f;
       float e = 0.14f;
       return Clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
   }

   RGB NullIntegrator::LiPixel(int row, int col) const {

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

       //int samples = 1;
       //acc_color[0] = 0.f;
       //acc_color[1] = 0.f;
       //acc_color[2] = 0.f;
       //for (int i = 0; i < samples; i++) {
           CameraSample cs;
           Point2f p = scene->sampler.Get2D();
           cs.pFilm.x = col + p.x;
           cs.pFilm.y = row + p.y;
           SampledWavelengths w;
           CameraRay r = scene->camera.GenerateRay(cs, w);

          // auto u = RadicalInverse(2, i);

           RGB final = Li(r.ray, 0.f);

           acc_color[0] += final.r;
           acc_color[1] += final.g;
           acc_color[2] += final.b;
      // }

#if 0
       pixel_color[0] = acc_color[0] / scene->SampleCount * 255;
       pixel_color[1] = acc_color[1] / scene->SampleCount * 255;
       pixel_color[2] = acc_color[2] / scene->SampleCount * 255;

#elif 1
        auto rr = acc_color[0] / scene->SampleCount;
        auto gg = acc_color[1] / scene->SampleCount;
        auto bb = acc_color[2] / scene->SampleCount;
        RGB beta = { rr,gg,bb };
        beta = Clamp(beta, 0, 1);
        pixel_color[0] = to_sRGB(beta.r) * 255;
        pixel_color[1] = to_sRGB(beta.g) * 255;
        pixel_color[2] = to_sRGB(beta.b) * 255;
#else
       auto rr = acc_color[0] / scene->SampleCount;
       auto gg = acc_color[1] / scene->SampleCount;
       auto bb = acc_color[2] / scene->SampleCount;

       RGB beta = ACESFilm({ rr,gg,bb });
       float gamma = 2.2f;
       beta.r = pow(beta.r, 1.0f / gamma);
       beta.g = pow(beta.g, 1.0f / gamma);
       beta.b = pow(beta.b, 1.0f / gamma);
       pixel_color[0] = beta.r * 255;
       pixel_color[1] = beta.g * 255;
       pixel_color[2] = beta.b * 255;
#endif
       return {};
   }

}