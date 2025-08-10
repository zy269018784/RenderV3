#pragma once
#include <cpu_gpu.h>
#include <Image/Image.h>
#include <Util/Frame.h>
#include <Intersection/SurfaceInteraction.h>
namespace Render {
    // Normal Mapping Function Definitions
    inline CPU_GPU void NormalMap(const Image& normalMap,
        const SurfaceInteraction& ctx, Vector3f* dpdu,
        Vector3f* dpdv) {
        // Get normalized normal vector from normal map
        WrapMode2D wrap(WrapMode::Repeat);
        Point2f uv(ctx.uv[0], 1 - ctx.uv[1]);
        Vector3f ns(2 * normalMap.BilerpChannel(uv, 0, wrap) - 1,
            2 * normalMap.BilerpChannel(uv, 1, wrap) - 1,
            2 * normalMap.BilerpChannel(uv, 2, wrap) - 1);
        ns = Normalize(ns);

        // Transform tangent-space normal to rendering space
        //Frame frame = Frame::FromZ(ctx.shading.n);
        Frame frame = Frame::FromZ(ctx.shading.n);
        ns = frame.FromLocal(ns);

        // Find $\dpdu$ and $\dpdv$ that give shading normal
        //Float ulen = Length(ctx.shading.dpdu), vlen = Length(ctx.shading.dpdv);
        //*dpdu = Normalize(GramSchmidt(ctx.shading.dpdu, ns)) * ulen;
        Float ulen = Length(ctx.shading.dpdu), vlen = Length(ctx.shading.dpdv);
        *dpdu = Normalize(GramSchmidt(ctx.shading.dpdu, ns)) * ulen;
        *dpdv = Normalize(Cross(ns, *dpdu)) * vlen;
    }
}