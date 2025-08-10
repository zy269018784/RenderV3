#pragma once
#include <string>
#include <vector>
#include <Util/Error.h>
#include <Math/Transform.h>
namespace Render {
    struct ObjectUVScaleDefinitionSceneEntity  {
        ObjectUVScaleDefinitionSceneEntity() = default;
        ObjectUVScaleDefinitionSceneEntity(const std::string& name, FileLoc loc) 
           : name(name), loc(loc) {}
        
        std::string name;   
        FileLoc loc;
        std::vector<std::string> shapeRefs;                 // 形状引用名称
        Point2f uvScale;      
        Point2f uvOffset;
        float uvRotation;
    };
}