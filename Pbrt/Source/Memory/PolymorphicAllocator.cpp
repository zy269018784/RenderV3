#include <Memory/CPU/CpuMemoryResource.h>

namespace Render {
    static CpuMemoryResource* ndr;

    MemoryResource* new_delete_resource() noexcept {
        if (!ndr)
            ndr = new CpuMemoryResource;
        return ndr;
    }
}