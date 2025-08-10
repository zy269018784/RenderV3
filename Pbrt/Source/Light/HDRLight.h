#pragma once
#include <Light/LightBase.h>
#include <Texture/RGB.h>
#include <Texture/SpectrumTexture.h>
#include <Shape/Sphere.h>
#include <Math/Sampling.h>
#include <Image/Image.h>
namespace Render {
    struct LightLiSample;
    struct SampledWavelengths;
    class LightSampleContext;
    class SpectrumTexture;
    class HDRLight : public LightBase {
    public:
        HDRLight(Transform worldFromLight, Float phi, Float theta, SpectrumTexture L, Image* image, Float power, Allocator alloc);

        static HDRLight* Create(const Transform& worldFromLight, Float phi, Float theta, SpectrumTexture L, Image* image, Float power,  Allocator alloc);
       
        void Preprocess(const Bounds3f& sceneBounds) {
            sceneBounds.BoundingSphere(&sceneCenter, &sceneRadius);
            // hack 场景半径放大100倍
            sceneRadius *= 100.f;
            Float m[4][4] = {
                   {1, 0, 0, -sceneCenter.x},
                   {0, 1, 0, -sceneCenter.y},
                   {0, 0, 1, -sceneCenter.z},
                   {0, 0, 0, 1}
            };
            Transform objectFromRender(m);
            Transform renderFromObject = objectFromRender.GetInverseMatrix();
            sceneSphere.set(renderFromObject, objectFromRender, sceneRadius);
            printf("Preprocess center %f %f %f, radius %f\n", sceneCenter.x, sceneCenter.y, sceneCenter.z, sceneRadius);
        }

        CPU_GPU LightLiSample SampleLi(
            LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
            bool allowIncompletePDF = false) const;

        CPU_GPU LightLiSample SampleRay(Ray r) const;

        CPU_GPU
            RGB ImageLe(Point2f uv, const SampledWavelengths& lambda) const;


    public:
        SpectrumTexture L;
        Float power; // 亮度
        Float phi;
        Float theta;
        Float scale;
        PiecewiseConstant2D distribution;
        PiecewiseConstant2D compensatedDistribution;
        Transform rotate;
    public:
        Point3f sceneCenter;    // 场景中心
        Float sceneRadius = 1.0;      // 场景包围球半径
        Sphere sceneSphere;
    };
}