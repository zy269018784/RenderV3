#pragma once 
#include <Math/Math.h>
#include <Texture/RGB.h>
namespace Render {
    struct BSDFSample {
        BSDFSample() = default;
        CPU_GPU
            BSDFSample(Float f, Vector3f wi, Float pdf,  Float eta = 1,
                bool pdfIsProportional = false)
            : f(f), 
            wi(wi),
            pdf(pdf),
            eta(eta),
            pdfIsProportional(pdfIsProportional) {}

        Float f;
        Vector3f wi;            // 局部坐标系 新方向
        Float pdf = 0;
        Float eta = 1;
        bool pdfIsProportional = false;
    };

}