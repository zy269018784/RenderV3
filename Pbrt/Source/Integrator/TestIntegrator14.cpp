#include <Integrator/TestIntegrator14.h>
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
#include <Light/HDRLight.h>
#include <CookTorrance/TrowbridgeReitzDistribution.h>
#include <BxDF/ConductorBxDF.h>
namespace Render {
    TestIntegrator14::TestIntegrator14(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }


    Optional<ShapeIntersection> TestIntegrator14::Intersect(const Ray& ray,
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

    static CPU_GPU Transform orthonormal_basis(Vector3f n)
    {
        Vector3f f, r;
        if (n.z < -0.999999)
        {
            f = Vector3f(0, -1, 0);
            r = Vector3f(-1, 0, 0);
        }
        else
        {
            Float a = 1. / (1. + n.z);
            Float b = -n.x * n.y * a;
            f = Normalize(Vector3f(1. - n.x * n.x * a, b, -n.x));
            r = Normalize(Vector3f(b, 1. - n.y * n.y * a, -n.y));
        }

        Float m[4][4];
        m[0][0] = f.x;
        m[0][1] = f.y;
        m[0][2] = f.z;
        m[0][3] = 0.f;
        m[1][0] = r.x;
        m[1][1] = r.y;
        m[1][2] = r.z;
        m[1][3] = 0.f;
        m[2][0] = n.x;
        m[2][1] = n.y;
        m[2][2] = n.z;
        m[2][3] = 0.f;
        m[3][0] = 0.f;
        m[3][1] = 0.f;
        m[3][2] = 0.f;
        m[3][3] = 1.f;

        Transform t(m);
        
        //return t;
        return Transpose(t);
    }

    static CPU_GPU float p22_ggx_anisotropic(float x, float y, float alpha_x, float alpha_y)
    {
        float x_sqr = x * x;
        float y_sqr = y * y;
        float alpha_x_sqr = alpha_x * alpha_x;
        float alpha_y_sqr = alpha_y * alpha_y;
        float denom = (1. + (x_sqr / alpha_x_sqr) + (y_sqr / alpha_y_sqr));
        float denom_sqr = denom * denom;
        return(
            1.
            / //-------------------------------------------------------------------
            ((Pi * alpha_x * alpha_y) * (denom_sqr))
            );
    }

    static CPU_GPU float ndf_ggx_anisotropic(Vector3f omega_h, float alpha_x, float alpha_y)
    {
        float slope_x = -(omega_h.x / omega_h.z);
        float slope_y = -(omega_h.y / omega_h.z);
        float cos_theta = CosTheta(omega_h);
        float cos_2_theta = cos_theta * cos_theta;
        float cos_4_theta = cos_2_theta * cos_2_theta;
        float ggx_p22 = p22_ggx_anisotropic(slope_x, slope_y, alpha_x, alpha_y);
        return(
            ggx_p22
            / //---------------------------
            cos_4_theta
            );
    }

    static CPU_GPU Float lambda_ggx_anisotropic(Vector3f omega, Float alpha_x, Float alpha_y)
    {
        Float cos_phi = CosPhi(omega);
        Float sin_phi = CosPhi(omega);
        Float alpha_o = std::sqrt(cos_phi * cos_phi * alpha_x * alpha_x + sin_phi * sin_phi * alpha_y * alpha_y);
        Float a = 1.f / (alpha_o * TanTheta(omega));
        return (0.5f * (-1. + std::sqrt(1.f + 1.f / (a * a))));
    }

    static CPU_GPU Vector3f fresnel_schlick(Float wo_dot_wh, Vector3f F0)
    {
        return F0 + Vector3f((1. - F0) * powf(1. - wo_dot_wh, 5.));
    }

    static CPU_GPU inline Float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) {
        Float f = nf * fPdf, g = ng * gPdf;
        if (IsInf(Sqr(f)))
            return 1;
        return Sqr(f) / (Sqr(f) + Sqr(g));
    }

    bool TestIntegrator14::IntersectP(const Ray& ray, Float tMax) const {
        auto intr = aggregate.Intersect(ray, tMax);
        if (intr->intr.material.Tag() == 3)
            return false;

        if (intr && intr->tHit < tMax) {
            //printf("intr \n");
            return true;
        }
        return false;
    };

    bool TestIntegrator14::Unoccluded(const SurfaceInteraction& p0, const SurfaceInteraction& p1)  const {
        Ray ray;
        ray.d = Normalize(p1.p() - p0.p());
        ray.o = p0.p();
        // return !IntersectP(ray, tMax); ShadowEpsilon = 0.0001f;
        return !IntersectP(ray, 1.0 - 0.0001f);
    };

     RGB TestIntegrator14::SampleLd(const LightSampleContext& ctx, const SurfaceInteraction& intr, const Ray& ray) const {
        RGB allLight(0.0, 0.0, 0.0);
       //RGB allLight(0.f, 0.f, 0.f);
        // 光照点的材质
       Material m = intr.material;
       // 光照点的BSDF
       SampledWavelengths lambda;
       BSDF bsdf = m.GetBSDF(intr, lambda);

       auto u = scene->sampler.Get2D();

       //if (u.x > 0.31 && u.x < 0.36 && u.y >0.6 && u.y < 0.62)printf("TestIntegrator14 u, %f %f \n", u.x, u.y);
       if (scene->hdrLight) {
           LightLiSample lightSample = scene->hdrLight->SampleLi(ctx, scene->sampler.Get2D(), {}, true);

           // Evaluate BSDF for light sample and check light visibility
           Vector3f wo = intr.wo, wi = lightSample.wi;
           auto f = bsdf.f(wo, wi).f * AbsDot(wi, intr.shading.n);
     
           if (!f || !Unoccluded(intr, lightSample.pLight)) { // 先放一放，感觉跟wi朝向有关
               return {};
          }

           // Return light's contribution to reflected radiance
          /* Float p_l = lightSample.p * lightSample.pdf;*/
           Float p_l = 1.0 * lightSample.pdf;
      
            Float p_b = bsdf.PDF(wo, wi);
            Float w_l = PowerHeuristic(1, p_l, 1, p_b);
           return lightSample.L * f;



            //return w_l * lightSample.L * f / p_l;
        
       }

        return allLight;
    }


    RGB TestIntegrator14::Li(const Ray& ray, Float u, Float tMax) const {
        RGB beta(1, 1, 1), L(0, 0, 0);
        while (1) {
            auto intr = Intersect(ray);
            if (intr) {
                Point2f u2 = scene->sampler.Get2D();
                auto bsdf = intr->intr.GetBSDF();
                //Point2f u2(std::sin(scene->SampleCount * 31233.413f) * 43758.5453123f, std::cos(scene->SampleCount * 43255.f) * 43758.5453123f);
                //u2 = { 0.0, 0.0 };
                LightSampleContext ctx(intr->intr);
         

                //Ray ray;
                auto Ld = SampleLd(ctx, intr->intr, ray);
 
                BSDFSample sample = bsdf.Sample_f(-ray.d, u, u2);
                beta *= intr->intr.material.Evaluate(intr->intr, {});
                L += (beta * Ld);
                auto wi = sample.wi;
                Vector3f n = intr->intr.shading.n;
                beta *= sample.f * AbsDot(wi, n) / sample.pdf;

                Ray ray2;
                ray2.d = Normalize(sample.wi);
                ray2.o = intr->intr.p();
            
                if (scene->hdrLight) {
                    auto sample = scene->hdrLight->SampleRay(ray2);
                    beta *= sample.L;
                    L += (beta);
                 
                    break;
                }
            
            } else  if (scene->hdrLight) {
                auto sample = scene->hdrLight->SampleRay(ray);
                beta *= sample.L;
                L += (beta);
                break;
            }
        }
        

        beta = L;
#if 0
        auto intr = Intersect(ray);
        if (intr) {
           // beta = intr->intr.material.Evaluate(intr->intr, {});
            beta = {1, 1 ,1};

            Vector3f wo = Normalize( - ray.d);
            auto bsdf = intr->intr.GetBSDF();
            //intr->intr.material.GetBSDF();
          //  auto woRender = Normalize(Vector3f(0.7, 0.9, 0.1));
            auto woRender = wo;
            wo = bsdf.ToLocal(wo); 

           TrowbridgeReitzDistribution a(0.1001f, 0.1001f);
           ConductorBxDF bxdf(a);
           auto sample = bxdf.Sample_f(wo, {}, {});
           auto wi = sample.wi;
           Vector3f n = bsdf.ToLocal(intr->intr.shading.n);
           beta *= sample.f * AbsDot(wi, n) / sample.pdf;
           
             //Vector3f n = intr->intr.shading.n;
             //auto wm = a.Sample_wm(wo, { 0, 0 });
             //wm = bsdf.ToLocal(wm);

             ////n = bsdf.ToLocal(n);
             //auto wi = Reflect(wo, wm);
             ////printf("D %f wo  %f  %f  %f wm  %f  %f  %f\n", a.D(wm), wo.x, wo.y, wo.z, wm.x, wm.y, wm.z);
             ///*auto D = a.D(wm);*/
             //auto D = a.D(wm);
             ////printf("D %f \n", D);
 
             //auto G = a.G(wo, wi);
             //auto pdf = a.PDF(wo, wm) / (4 * AbsDot(wo, wm));
             //auto F = 1.f;
             //F = FrDielectric(Dot(wo, wm), 0.1f);
             //Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
             ////beta *= D * G * F / (4 * cosTheta_i * cosTheta_o);
             //beta *= D * G * F;

        }
#endif    
        return beta;
    }



    RGB TestIntegrator14::LiPixel(int row, int col) const {

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
        scene->sampler.StartPixelSample({row, col}, scene->SampleCount); 
        CameraSample cs;
        Point2f p = scene->sampler.Get2D(); //gpu同一个wrap使用的变量可能是同一个p
        cs.pFilm.x = col + p.x;
        cs.pFilm.y = row + p.y;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        RGB final(0, 0, 0);
        //if (row == scene->renderBuffer->Width() / 2 && col == scene->renderBuffer->Height() - 10)
        final = Li(r.ray, 0.f);

        acc_color[0] += final.r;
        acc_color[1] += final.g;
        acc_color[2] += final.b;
        //if(acc_color[0]>10.0)printf("acc_color %f %f %f\n", acc_color[0], acc_color[1], acc_color[2]);

        //pixel_color[0] = acc_color[0] / scene->SampleCount * 255;
        //pixel_color[1] = acc_color[1] / scene->SampleCount * 255;
        //pixel_color[2] = acc_color[2] / scene->SampleCount * 255;


        //acc_color[0] += final.r;
        //acc_color[1] += final.g;
        //acc_color[2] += final.b;

        auto rr = acc_color[0] / scene->SampleCount;
        auto gg = acc_color[1] / scene->SampleCount;
        auto bb = acc_color[2] / scene->SampleCount;

        RGB beta = ACESFilm({ rr,gg,bb });
        pixel_color[0] = beta.r * 255;
        pixel_color[1] = beta.g * 255;
        pixel_color[2] = beta.b * 255;


        return {};
    }
}