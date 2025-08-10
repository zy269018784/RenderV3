#include <Material/Material.h>
#include <Material/DiffuseMaterial.h>
#include <Material/DielectricMaterial.h>
#include <Material/ConductorMaterial.h>
#include <Material/GlassMaterial.h>
#include <Material/MirrorMaterial.h>
#include <Material/GraniteMaterial.h>
#include <Camera/SampledWavelengths.h>
#include <Texture/MaterialEvalContext.h>
#include <Texture/UniversalTextureEvaluator.h>
#include <BxDF/BSDF.h>

namespace Render {
    CPU_GPU RGBA Material::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const {
        auto eval = [&](auto ptr) { return ptr->Evaluate(ctx, lambda); };
        return Dispatch(eval);
    }

    CPU_GPU BSDF Material::GetBSDF(MaterialEvalContext ctx, SampledWavelengths& lambda) {
        auto getBSDF = [&](auto mtl) -> BSDF {
            using ConcreteMtl = typename std::remove_reference_t<decltype(*mtl)>;
            using ConcreteBxDF = typename ConcreteMtl::BxDF;

            if constexpr (std::is_same_v<ConcreteBxDF, void>) 
                return BSDF();

            //RGB color = mtl->Evaluate(ctx, lambda);
            //color /= 255.f;
            ConcreteBxDF* bxdf = mtl->GetBxDF(ctx, lambda);
            //return BSDF(ctx.ns, ctx.dpdus, bxdf, color);
            return BSDF(ctx.ns, ctx.dpdus, bxdf);
        };

        return Dispatch(getBSDF);
    }

    CPU_GPU const Image* Material::GetNormalMap() const {
        auto nmap = [&](auto ptr) { return ptr->GetNormalMap(); };
        return Dispatch(nmap);
    }
}
