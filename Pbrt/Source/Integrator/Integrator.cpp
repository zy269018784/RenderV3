#include <Integrator/Integrator.h>
#include <Integrator/TestIntegrator.h>
#include <Integrator/TestIntegrator2.h>
#include <Integrator/TestIntegrator3.h>
#include <Integrator/TestIntegrator4.h>
#include <Integrator/TestIntegrator5.h>
#include <Integrator/TestIntegrator6.h>
#include <Integrator/TestIntegrator7.h>
#include <Integrator/TestIntegrator8.h>
#include <Integrator/TestIntegrator9.h>
#include <Integrator/TestIntegrator10.h>
#include <Integrator/TestIntegrator11.h>
#include <Integrator/TestIntegrator12.h>
#include <Integrator/TestIntegrator13.h>
#include <Integrator/TestIntegrator14.h>
#include <Integrator/TestGGX.h>
#include <Integrator/TestHDR.h>
#include <Integrator/TestTextureIntegrator.h>
#include <Integrator/PathIntegrator.h>
#include <Integrator/WaterIntegrator.h>
#include <Integrator/NullIntegrator.h>
#include <Integrator/GlassIntegrator.h>
#include <Integrator/GlassIntegrator2.h>
#include <Texture/RGB.h>
#include <Texture/TextureEvalContext.h>
#include <Camera/SampledWavelengths.h>
#include <Intersection/Intersection.h>
#include <Ray/Ray.h>

namespace Render {

    Optional<ShapeIntersection> Integrator::Intersect(const Ray& ray,
        Float tMax) const {
        auto Intersect = [&](auto ptr) { return ptr->Intersect(ray, tMax); };
        return Dispatch(Intersect);
    }

    RGB Integrator::Li(const Ray& ray, Float u, Float tMax) const { 
        auto Li = [&](auto ptr) { return ptr->Li(ray, u, tMax); };
        return Dispatch(Li);
    }

    RGB Integrator::LiPixel(int row, int col) const {
        auto LiPixel = [&](auto ptr) { return ptr->LiPixel(row, col); };
        return Dispatch(LiPixel);
    }

   //Optional<ShapeIntersection> Integrator::Intersect(const Ray& ray,
   //    Float tMax) const {
   //    if (aggregate)
   //        return aggregate.Intersect(ray, tMax);
   //}
	//
   //
   //RGB Integrator::Li(const Ray& ray, Float tMax) const {
   //    RGB color(0, 0, 0);
   //    Optional<ShapeIntersection> intr = Intersect(ray, tMax);
   //    if (intr) {
   //        auto tag = intr->intr.material.Tag();
   //        if (tag > 0) {
   //            color = intr->intr.material.Evaluate(intr->intr, {});
   //        }
   //    }
   //    return color;
   //}
}