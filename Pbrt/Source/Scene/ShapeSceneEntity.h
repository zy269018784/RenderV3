#pragma once
#include <Scene/SceneEntity.h>
#include <Math/Transform.h>
namespace Render {
    struct ShapeSceneEntity : public SceneEntity {
    public:
        ShapeSceneEntity() = default;
        ShapeSceneEntity(const std::string& name, ParameterDictionary parameters, FileLoc loc,
            int materialIndex, const std::string& materialName)
            : SceneEntity(name, parameters, loc),
            materialIndex(materialIndex),
            materialName(materialName) {}

    public:
        int materialIndex;  
        std::string materialName;
    };
}