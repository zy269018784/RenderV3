#include <Primitive/KdTreeAggregate.h>

namespace Render {
    // SimplePrimitive Definition

    Bounds3f KdTreeAggregate::Bounds() const
    {
        return Bounds3f(Point3f(432.f, 111.f, 333.f));
        return {};
    }
}