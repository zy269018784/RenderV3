#pragma once
#include <Math/Math.h>
#include <Texture/TextureEvalContext.h>
#include <Intersection/SurfaceInteraction.h>
namespace Render {
    // MaterialEvalContext Definition
    struct MaterialEvalContext : public TextureEvalContext {
        // MaterialEvalContext Public Methods
        MaterialEvalContext() = default;
        CPU_GPU
            MaterialEvalContext(const SurfaceInteraction& si)
            : TextureEvalContext(si), wo(si.wo), ns(si.shading.n), dpdus(si.shading.dpdu) {}

        Vector3f wo;
        Normal3f ns;
        Vector3f dpdus;
    };
}