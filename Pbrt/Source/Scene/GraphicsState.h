#pragma once
#include <Math/Transform.h>
#include <string>
namespace Render {
    struct GraphicsState {
        GraphicsState();

        int currentMaterialIndex = -1;
        std::string currentMaterialName;

        int currentShapeIndex = -1;
        std::string currentShapeName;

        int currentObjectIndex = -1;
        std::string currentObjectName;
        Transform   ctm;
    };

}