#pragma once
#include <string>
#include <vector>
#include <Util/Error.h>
#include <Math/Transform.h>
namespace Render {
    struct ObjectDefinitionSceneEntity  {
        ObjectDefinitionSceneEntity() = default;
        ObjectDefinitionSceneEntity(const std::string& name, FileLoc loc) 
            : name(name), loc(loc) {}

        std::string name;                                   // 对象名称
        FileLoc loc;
        std::vector<std::string> shapeRefs;                 // 形状引用名称
        std::vector<std::string> materialsNames;            // 材质名称
        std::vector<Transform>   transforms;                // 转换矩阵
    };
}