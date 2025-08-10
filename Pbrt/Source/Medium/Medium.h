#pragma once
#include <cpu_gpu.h>
#include <Util/TaggedPointer.h>
#include <Medium/HomogeneousMedium.h>
namespace Render {
    class Medium
        : public TaggedPointer<HomogeneousMedium> {
    public:
        using TaggedPointer::TaggedPointer;
    };
}