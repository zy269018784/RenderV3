#pragma once
#include <Math/Math.h>
#include <Type/Types.h>
namespace Render {
    struct CameraSample {
        Point2f pFilm;
        Point2f pLens;
        Float time = 0;
        Float filterWeight = 1;
    };
}