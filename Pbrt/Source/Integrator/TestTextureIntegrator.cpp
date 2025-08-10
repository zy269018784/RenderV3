#include <Integrator/TestTextureIntegrator.h>
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
    TestTextureIntegrator::TestTextureIntegrator(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    Optional<ShapeIntersection> TestTextureIntegrator::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    // https://www.shadertoy.com/view/MlSSR1
    // https://www.shadertoy.com/view/ltlSWB
    RGB TestTextureIntegrator::Li(const Ray& ray, Float u, Float tMax) const {

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

    RGB TestTextureIntegrator::LiPixel(int row, int col) const {

        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
 
        Float u = row * 1.f / Width  * 2.f;
        Float v = col * 1.f / Height * 2.f;

        TextureEvalContext ctx;
        ctx.uv.x = 1.0f * col / Width;
        ctx.uv.y = 1.0f * row / Height;
        SampledWavelengths sw;
        RGB c;// = scene->textures[0].Evaluate(ctx, sw);

        if (0 == col && 0 == row) {
            printf("rgb %f %f %f\n", c.r, c.g, c.b);
        }

        pixel_color[0] = c.r * 255;
        pixel_color[1] = c.g * 255;
        pixel_color[2] = c.b * 255;

        return {};
    }
}